//this is the base class
#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdarg.h>
#include <math.h>
#include "mpi_npu_api.h"
#include "cJSON.h"
#include "hal_sys.h"
#include "opencv2/opencv.hpp"
#include "mpi_scaler_api.h"

using namespace std;
using namespace cv;

#include "armtcnn_base.h"
#include "armtcnn_common.h"

#define AR_MPI_NPU_DEBUG
//#define AR_SCALER_DEBUG
#define ARMTCNN_PNET_START_INDEX   0
#define AR_SCALER_SINGLE_MODE      1
//#define SCALER_MATLAB_TEST 1

//[w,h]
#define IDW 0
#define IDH 1
//int final_box[19][2] =
//{
//    {91, 28},
//    {9, 11},
//    {240, 68},
//    {75, 35},
//    {72, 25},
//    {63, 35},
//    {11, 35},
//    {189, 83},
//    {273, 74},
//    {124, 43},
//    {79, 46},
//    {201, 54},
//    {225, 71},
//    {229, 62},
//    {122, 54},
//    {86, 39},
//    {29, 26},
//    {301, 85},
//    {160, 53},
//};

extern "C" AR_S32 DumpIFCOutput(void * handle, void * pIfcOutput);
extern "C" AR_S32 DumpLayerData(void * handle, AR_U32 u32CurrLayer);


static int CompareScore(BONDING_BOX_S & box1, BONDING_BOX_S & box2)
{
    return(box1.dScore > box2.dScore);
}

CARMTCNN::CARMTCNN()
{
    u32Initialized = 0;
    u32LogLevelCtrl = 5;
	handle =NULL;
}

AR_S32 CARMTCNN::Initialize(AR_CHAR * pchCfgFile)
{
    AR_S32 ret=0;
    AR_CHAR * pchBuff = NULL;
    AR_U32 u32Size;

    u32Initialized = 0;
    memset(&stCNNDesc, 0, sizeof(stCNNDesc));
    handle = 0;
    u32IfcPreProcess = 0;
    u32ProfileEnable = 0;

    memset(astProfile, 0, sizeof(astProfile));

    if(!pchCfgFile)
    {
        Log(ARCNN_LOG_ERR, "Invalid json file name.\n");
        return -1;
    }

    ifstream cfgFile(pchCfgFile, ios::in|ios::binary|ios::ate);
    if(!cfgFile.is_open())
    {
        Log(ARCNN_LOG_ERR, "Open json file failed.\n");
        return -1;
    }

    u32Size = cfgFile.tellg();
    cfgFile.seekg(0, ios::beg);
    pchBuff = new char[u32Size];
    if(!pchBuff)
    {
        Log(ARCNN_LOG_ERR, "Get buffer error in constructor.\n");
        return -1;
    }

    cfgFile.read(pchBuff, u32Size);
    cfgFile.close();

    cJSON * pJson = NULL;
    cJSON * pNet = NULL;
    pJson = cJSON_Parse(pchBuff);
    if(!pJson)
    {
        Log(ARCNN_LOG_ERR, "Parse cfg json file[%s] failed.\n", pchCfgFile);
        delete[] pchBuff;
        return -1;
    }

    pNet = cJSON_GetObjectItemCaseSensitive(pJson, "network");
    if(!pNet)
    {
        Log(ARCNN_LOG_ERR, "Parse network node failed.\n");
        delete[] pchBuff;
        cJSON_Delete(pJson);
        return -1;
    }

    cJSON * id = cJSON_GetObjectItemCaseSensitive(pNet, "id");
    if(!id)
    {
        Log(ARCNN_LOG_ERR, "Parse network id failed.\n");
        //set default id
        stCNNDesc.u16NetworkID = 0xDEFA;
    }
    else
    {
        stCNNDesc.u16NetworkID = atoi(id->valuestring);
    }

    cJSON * priority = cJSON_GetObjectItemCaseSensitive(pNet, "priority");
    if(priority && strcmp(priority->valuestring, "high") == 0)
    {
        stCNNDesc.u32Priority = NETWORK_PRIORITY_HIGH;
    }
    else
    {
        stCNNDesc.u32Priority = NETWORK_PRIORITY_NORMAL;
    }


	cJSON * preproc = cJSON_GetObjectItemCaseSensitive(pNet, "ifc-pre-process");
	if(preproc && strcmp(preproc->valuestring, "yes") == 0)
	{
		 u32IfcPreProcess = 1;
	}
	else
	{
		 u32IfcPreProcess = 0;
	}

	cJSON * npufile = cJSON_GetObjectItemCaseSensitive(pNet, "npubin");
	if(!npufile)
	{
		Log(ARCNN_LOG_ERR, "Parse npubin node failed.\n");
		delete[] pchBuff;
		cJSON_Delete(pJson);
		return -1;
	}
	else
	{
		strcpy(stCNNDesc.au8NpubinFileName, npufile->valuestring);
		Log(ARCNN_LOG_INFO, "au8NpubinFileName=%s\n",stCNNDesc.au8NpubinFileName);
	}


    cJSON * cbfile = cJSON_GetObjectItemCaseSensitive(pNet, "callback_to_arm");
    if(!cbfile || strcmp(cbfile->valuestring, "yes") == 0)
    {
        stCNNDesc.u32CBToArm = 1;
    }
    else
    {
        stCNNDesc.u32CBToArm = 0;
    }

    cJSON * sramaddr = cJSON_GetObjectItemCaseSensitive(pNet, "sram_addr");
    if(!sramaddr)
    {
        stCNNDesc.u32SramAddrPhy = 0;
		ret = AR_MPI_NPU_SetSecurity(0);
		if(ret<0)
		{
			Log(ARCNN_LOG_ERR, "ar_hal_npu_set_security failed.\n");
		}
    }
    else
    {
        stCNNDesc.u32SramAddrPhy = strtoul(sramaddr->valuestring, NULL, 16);
        cJSON * sramsize = cJSON_GetObjectItemCaseSensitive(pNet, "sram_size");
        if(!sramsize)
        {
            stCNNDesc.u32SramSize = 0;
			ret = AR_MPI_NPU_SetSecurity(0);
			if(ret<0)
			{
				Log(ARCNN_LOG_ERR, "ar_hal_npu_set_security failed.\n");
			}

        }
        else
        {
            stCNNDesc.u32SramSize = strtoul(sramsize->valuestring, NULL, 16);
			ret = AR_MPI_NPU_SetSecurity(1);
			if(ret<0)
			{
				Log(ARCNN_LOG_ERR, "ar_hal_npu_set_security failed.\n");
			}
        }
    }

    delete[] pchBuff;
    cJSON_Delete(pJson);

    //Loadmodel
    handle = AR_MPI_NPU_LoadModel(&stCNNDesc);
    if(!handle)
    {
        Log(ARCNN_LOG_ERR, "Load Model failed.\n");
        return -1;
    }

    MallocNPUBuff();

    u32Initialized = 1;
    return 0;
}

CARMTCNN::~CARMTCNN()
{
    //unload model
    if(handle)
    {
        FreeNPUBuff();
        AR_MPI_NPU_UnloadModel(handle);
    }

    handle = NULL;
}

AR_S32 CARMTCNN::SetThreshold(AR_DOUBLE fThresh)
{
    fScoreThresh = fThresh;

    return 0;
}

AR_S32 CARMTCNN::SetScaleParam(AR_DOUBLE dScale, AR_U32 u32W, AR_U32 u32H, AR_U32 u32SrcW, AR_U32 u32SrcH)
{
    dPnetScale = dScale;
    u32Width = u32W;
    u32Height = u32H;
    u32SrcWidth = u32SrcW;
    u32SrcHeight = u32SrcH;

    return 0;
}

AR_S32 CARMTCNN::Forward(AR_IMG_SET_S * pstImg, AR_BOOL bInstant, AR_BOOL bDebug)
{
    ProfileStart("Forward");
    AR_S32 s32Ret = AR_MPI_NPU_Forward(handle, pstImg, &stNPUInBuff, &stNPUOutBuff, bInstant, bDebug);
    ProfileEnd("Forward");

#ifdef AR_MPI_NPU_DEBUG
    char tmp[128] = {0};
    sprintf(tmp, "./net%d_dla_output.bin", AR_MPI_NPU_GetNetworkId(handle));
    DumpOutput(tmp);
#endif
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Forward failed.\n");
        return s32Ret;
    }

    return s32Ret;
}

AR_S32 CARMTCNN::PostProcess(AR_U32 u32PostMode)
{
    //get output and put in a list, tensor 0 is box, tensor 1 is landmark (if available), tensor last is score
    //x0,y0 = stride *()/scale
    int indexConv = 0;
    //int indexLandmark = 1;
    int indexProb = 0;//handle->pstCNNModel->u32OutputTensorNum - 1;// 1 or 2
    AR_NPU_TENSOR_S stTensorProb;
    AR_NPU_TENSOR_S stTensorConv;
    AR_NPU_TENSOR_S stTensorZero;

    indexProb = AR_MPI_NPU_GetOutputTensorNum(handle);
    AR_MPI_NPU_GetOutputTensorParam(handle, indexProb-1, &stTensorProb);
    AR_MPI_NPU_GetOutputTensorParam(handle, indexConv, &stTensorConv);
    AR_MPI_NPU_GetOutputTensorParam(handle, 0, &stTensorZero);

    AR_U32 u32W = stTensorProb.u32Width;
    AR_U32 u32H = stTensorProb.u32Height;
    AR_U32 u32RowStepProb = stTensorProb.u32RowStep;
    AR_U32 u32RowStepConv = stTensorZero.u32RowStep;
    AR_U32 u32CProb = stTensorProb.u32OriChannels;
    AR_U32 u32CConv = stTensorConv.u32OriChannels;

    AR_UINTPTR uptrOutputAddr = stNPUOutBuff.u64VirtAddr;
    //AR_U32	   u32Size = AR_MPI_NPU_GetOutputBuffSize(handle);

    AR_CHAR * pu8Prob = (AR_CHAR *)uptrOutputAddr + stTensorProb.u32Offset;
    AR_CHAR * pu8Conv = (AR_CHAR *)uptrOutputAddr + stTensorConv.u32Offset;
    AR_U32 u32Precision = stTensorConv.u32Precision;
    //AR_DOUBLE dFactorProb = stTensorProb.dScaleFactor;
    //AR_S32  s32ZeroPointProb = stTensorProb.s32ZeroPoint;
    AR_DOUBLE dFactorConv = stTensorConv.dScaleFactor;
    AR_S32  s32ZeroPointConv = stTensorConv.s32ZeroPoint;
    AR_U32 u32BytePerChan = u32Precision / 8;
    AR_U32 u32BatchNum = AR_MPI_NPU_GetBatchNum(handle);
    AR_U32 u32TensorStepProb = stTensorProb.u32TensorStep;
    AR_U32 u32TensorStepConv = stTensorConv.u32TensorStep;

    AR_U32 u32RowStepLandmark = 0;
    AR_U32 u32CLandmark = 0;
    AR_CHAR * pu8Landmark = 0;
    AR_DOUBLE dFactorLandmark = 0;
    AR_S32 s32ZeroPointLandmark = 0;
    AR_U32 u32TensorStepLandmark = 0;

    //int cnt = 0;
    printf("Start post process with thresh %f, prob at %d conv at %d...\n", fScoreThresh, indexProb, indexConv);

    /////////debug
    char tmp[128] = {0};
    sprintf(tmp, "./net%d_post.txt", AR_MPI_NPU_GetNetworkId(handle));
    ofstream dbgp(tmp);

    //clear the bonding box first.
    listBondingBox.clear();

    AR_U32 u32WithLandmark = (AR_MPI_NPU_GetOutputTensorNum(handle) == 3);
    if(u32WithLandmark && u32PostMode == ARMTCNN_POST_MODE_ONET)
    {
        AR_NPU_TENSOR_S stTensorLandmark;
	    AR_MPI_NPU_GetOutputTensorParam(handle, u32WithLandmark, &stTensorLandmark);
        u32RowStepLandmark = stTensorLandmark.u32RowStep;
        u32CLandmark = stTensorLandmark.u32OriChannels;
        pu8Landmark = (AR_CHAR *)uptrOutputAddr + stTensorLandmark.u32Offset;
        dFactorLandmark = stTensorLandmark.dScaleFactor;
        s32ZeroPointLandmark = stTensorLandmark.s32ZeroPoint;
        u32TensorStepLandmark = stTensorLandmark.u32TensorStep;
    }

    BONDING_BOX_S stBondingBox = {0};
    AR_NPU_TENSOR_S stTensorOne;
    AR_MPI_NPU_GetOutputTensorParam(handle, 1, &stTensorOne);
    if(strcmp(stTensorOne.achType,"integer") == 0)
    {
        if(u32Precision == 16)
        {
            for(AR_U32 b = 0; b < u32BatchNum; b++)
            {
                for(AR_U32 h = 0; h < u32H; h++)
                {
                    for(AR_U32 w = 0; w < u32W; w++)
                    {
                        AR_U16 u16Prob0 = *(AR_S16 *)(pu8Prob + b * u32TensorStepProb + h * u32RowStepProb + w * u32CProb * u32BytePerChan);
                        AR_U16 u16Prob1 = *(AR_S16 *)(pu8Prob + b * u32TensorStepProb + h * u32RowStepProb + w * u32CProb * u32BytePerChan + u32BytePerChan);
                        AR_DOUBLE dProb = (AR_DOUBLE)u16Prob1 / (AR_DOUBLE)(u16Prob0 + u16Prob1);
                        memset(&stBondingBox, 0, sizeof(stBondingBox));

                        if(dProb > fScoreThresh)
                        {
                            //this is a valid box
                            stBondingBox.u32Valid = 1;
                            if(u32PostMode == ARMTCNN_POST_MODE_PNET)
                            {
                                stBondingBox.fX0 = int(GET_BBOX_TOP(w, dPnetScale)) - 1;
                                stBondingBox.fY0 = int(GET_BBOX_TOP(h, dPnetScale)) - 1;
                                stBondingBox.fX1 = int(GET_BBOX_BOTTOM(w, dPnetScale)) - 1;
                                stBondingBox.fY1 = int(GET_BBOX_BOTTOM(h, dPnetScale)) - 1;

                                //stBondingBox.fX0 = CAP(stBondingBox.X0, 0, u32SrcWidth - 1);
                                //stBondingBox.fY0 = CAP(stBondingBox.Y0, 0, u32SrcHeight - 1);
                                //stBondingBox.fX1 = CAP(stBondingBox.X1, 0, u32SrcWidth - 1);
                                //stBondingBox.fY1 = CAP(stBondingBox.Y1, 0, u32SrcHeight - 1);
                                stBondingBox.u32IndexH = h + 1;
                                stBondingBox.u32IndexW = w + 1;
                            }
                            stBondingBox.dScore = dProb;

                            AR_S16 * ps16Conv = (AR_S16 *)(pu8Conv + b * u32TensorStepConv + h * u32RowStepConv + w * u32CConv * u32BytePerChan);
                            stBondingBox.fDx0 = FIX_TO_FLOAT(*ps16Conv, s32ZeroPointConv, dFactorConv);
                            stBondingBox.fDy0 = FIX_TO_FLOAT(*(ps16Conv + 1), s32ZeroPointConv, dFactorConv);
                            stBondingBox.fDx1 = FIX_TO_FLOAT(*(ps16Conv + 2), s32ZeroPointConv, dFactorConv);
                            stBondingBox.fDy1 = FIX_TO_FLOAT(*(ps16Conv + 3), s32ZeroPointConv, dFactorConv);

                            //TBD: get landmarks
                            if(u32WithLandmark && u32PostMode == ARMTCNN_POST_MODE_ONET)
                            {
                                AR_S16 * ps16Landmark = (AR_S16 *)(pu8Landmark + b * u32TensorStepLandmark + h * u32RowStepLandmark + w * u32CLandmark * u32BytePerChan);
                                for(AR_U32 i = 0; i < u32CLandmark; i++)
                                {
                                    stBondingBox.fLandmark[i] = (*(ps16Landmark + i) - s32ZeroPointLandmark) * dFactorLandmark;
                                }
                            }

                            dbgp << " b,w,h: " << b << " " << w << " " << h << " Score " << dProb
                                 << " [ " << stBondingBox.fX0 << " " << stBondingBox.fY0 << " " << stBondingBox.fX1 << " " << stBondingBox.fY1 <<"]"
                                 << " [ " << stBondingBox.fDx0 << " " << stBondingBox.fDy0 << " " << stBondingBox.fDx1 << " " <<stBondingBox.fDy1 <<"]" << endl;

                            listBondingBox.push_back(stBondingBox);
                        }
                        else
                        {
                            //for rnet/onet, we still need the invalid box, this will be used to filter Pnet_total_bbox list
                            if(u32PostMode == ARMTCNN_POST_MODE_RNET || u32PostMode == ARMTCNN_POST_MODE_ONET)
                            {
                                stBondingBox.u32Valid = 0;
                                stBondingBox.dScore = dProb; //only for uplayer to check it

                                //std::cout << "ROnet set invalid: batch: " << b << " Score " << stBondingBox.fScore << endl;

                                listBondingBox.push_back(stBondingBox);
                            }
                        }
                    }
                }
            }
        }
        else //8bit precesion
        {
            for(AR_U32 b = 0; b < u32BatchNum; b++)
            {
                for(AR_U32 h = 0; h < u32H; h++)
                {
                    for(AR_U32 w = 0; w < u32W; w++)
                    {
                        AR_CHAR u8Prob0 = *(signed char *)(pu8Prob + b * u32TensorStepProb + h * u32RowStepProb + w * u32CProb * u32BytePerChan);
                        AR_CHAR u8Prob1 = *(signed char *)(pu8Prob + b * u32TensorStepProb + h * u32RowStepProb + w * u32CProb * u32BytePerChan + u32BytePerChan);
                        AR_DOUBLE dProb = (AR_DOUBLE)u8Prob1 / (AR_DOUBLE)(u8Prob0 + u8Prob1);
                        memset(&stBondingBox, 0, sizeof(stBondingBox));

                        if(dProb > fScoreThresh)
                        {
                            //this is a valid box
                            stBondingBox.u32Valid = 1;
                            if(u32PostMode == ARMTCNN_POST_MODE_PNET)
                            {
                                stBondingBox.fX0 = int(GET_BBOX_TOP(w + 1, dPnetScale)) - 1;
                                stBondingBox.fY0 = int(GET_BBOX_TOP(h + 1, dPnetScale)) - 1;
                                stBondingBox.fX1 = int(GET_BBOX_BOTTOM(w + 1, dPnetScale)) - 1;
                                stBondingBox.fY1 = int(GET_BBOX_BOTTOM(h + 1, dPnetScale)) - 1;
                                //stBondingBox.fX0 = CAP(stBondingBox.fX0, 0, u32SrcWidth - 1);
                                //stBondingBox.fY0 = CAP(stBondingBox.fY0, 0, u32SrcHeight -1);
                                //stBondingBox.fX1 = CAP(stBondingBox.fX1, 0, u32SrcWidth - 1);
                                //stBondingBox.fY1 = CAP(stBondingBox.fY1, 0, u32SrcHeight - 1);
                            }
                            stBondingBox.dScore = dProb;
                            //BBOX regression
                            signed char * ps8Conv = (signed char *)(pu8Conv + b * u32TensorStepConv + h * u32RowStepConv + w * u32CConv * u32BytePerChan);
                            stBondingBox.fDx0 = *ps8Conv;
                            stBondingBox.fDy0 = *(ps8Conv + 1);
                            stBondingBox.fDx1 = *(ps8Conv + 2);
                            stBondingBox.fDy1 = *(ps8Conv + 3);

                            //TBD: get landmarks
                            if(u32WithLandmark && u32PostMode == ARMTCNN_POST_MODE_ONET)
                            {
                                signed char * ps8Landmark = (signed char *)(pu8Landmark + b * u32TensorStepLandmark + h * u32RowStepLandmark + w * u32CLandmark * u32BytePerChan);
                                for(AR_U32 i = 0; i < u32CLandmark; i++)
                                {
                                    stBondingBox.fLandmark[i] = (*(ps8Landmark + i) - s32ZeroPointLandmark) * dFactorLandmark;
                                }
                            }

                            listBondingBox.push_back(stBondingBox);
                        }
                        else
                        {
                            //for rnet/onet, we still need the invalid box, this will be used to filter Pnet_total_bbox list
                            if(u32PostMode == ARMTCNN_POST_MODE_RNET || u32PostMode == ARMTCNN_POST_MODE_ONET)
                            {
                                stBondingBox.u32Valid = 0;
                                stBondingBox.dScore = dProb;
                                listBondingBox.push_back(stBondingBox);
                            }
                        }
                    }
                }
            }
        }
    }
    else //float output, no need to do scale
    {
        Log(ARCNN_LOG_INFO, "Not implemented %s output yet.\n", stTensorOne.achType);
    }

    return 0;
}

AR_S32 CARMTCNN::Nms(AR_DOUBLE fThresh)
{
    listBondingBox.sort(CompareScore);
    CalcNMS(listBondingBox, fThresh, ARMTCNN_NMS_UNION);
    /////////debug
    char tmp[128] = {0};

    sprintf(tmp, "./net%d_nms.txt", AR_MPI_NPU_GetNetworkId(handle));
    ofstream dbgp(tmp);

    for(BondingBoxList::iterator it = listBondingBox.begin(); it != listBondingBox.end(); it++)
    {
        dbgp << "w/h: " << it->u32IndexW - 1 << " " << it->u32IndexH -1 << " Score " << it->dScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<"]" << endl;
    }
	return 0;
}

AR_U32 CARMTCNN::GetBondingBoxNum()
{
    return listBondingBox.size();
}

AR_U32 CARMTCNN:: GetIfcPreProcess()
{
	return u32IfcPreProcess;
}

BONDING_BOX_S CARMTCNN::GetBondingBox()
{
    BONDING_BOX_S stBox;
    stBox = listBondingBox.front();
    listBondingBox.pop_front();

    return stBox;
}

AR_U32 CARMTCNN::GetBatchNum( )
{
    return AR_MPI_NPU_GetBatchNum(handle);
}

//here, we pass the input image name to generate a output file.
AR_S32 CARMTCNN::DumpOutput(AR_CHAR * pFile)
{
    AR_S32 s32Ret = 0;
    AR_U32 size = 0;
    AR_UINTPTR outputBuff = stNPUOutBuff.u64VirtAddr;

    size = AR_MPI_NPU_GetOutputBuffSize(handle);

    Log(ARCNN_LOG_INFO,"Output %lx: %x %x %x %x\n",outputBuff, *(AR_U32 *)outputBuff, *(AR_U32 *)(outputBuff + 4), *(AR_U32 *)(outputBuff + 8), *(AR_U32 *)(outputBuff + 12));

    ofstream ofile(pFile, ios::binary);
    if(ofile)
    {
        ofile.write((AR_CHAR *)outputBuff, size);
        Log(ARCNN_LOG_DEBUG, "Save output to %s finished.\n", pFile);
    }
    ofile.flush();
    ofile.close();

    return s32Ret;
}

AR_S32 CARMTCNN::Log(AR_U32 u32LogLevel, const AR_CHAR * fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char tmp[1024] = {0};
    vsnprintf(tmp, 1024, fmt, args);

    if(u32LogLevelCtrl >= u32LogLevel)
    {
        printf("[ARMTCNN][%s]: %s\n",
            (u32LogLevel == ARCNN_LOG_ERR)?"ERR":
            (u32LogLevel == ARCNN_LOG_WARNING)?"WARN":
            (u32LogLevel == ARCNN_LOG_DEBUG)?"DBG":"INFO",
             tmp);
    }

    va_end(args);
    return 0;
}

AR_S32 CARMTCNN::SetLogLevel(AR_U32 u32LogLevel)
{
    u32LogLevelCtrl = u32LogLevel;

    return 0;
}

AR_S32 CARMTCNN::EnableProfile()
{
    u32ProfileEnable = 1;
    return 0;
}

AR_S32 CARMTCNN::DisableProfile()
{
    u32ProfileEnable = 0;
    return 0;
}

AR_S32 CARMTCNN::ProfileStart(string pchName)
{
    AR_U32 i = 0;

    if(u32ProfileEnable)
    {
        for(i = 0; i < ARCNN_MAX_PROFILE; i++)
        {
            if(astProfile[i].strName.empty())
                break;

            if(astProfile[i].strName == pchName)
                break;
        }

        if(i == ARCNN_MAX_PROFILE)
        {
            Log(ARCNN_LOG_ERR, "Profile has run out.\n");
            return 0;
        }

        astProfile[i].strName = pchName;
        struct timeval tm;
        gettimeofday(&tm, NULL);
        astProfile[i].u64Tic = tm.tv_sec * 1000 + tm.tv_usec / 1000;
    }

    return 0;
}

AR_S32 CARMTCNN::ProfileEnd(string pchName)
{
    AR_U32 i = 0;

    if(u32ProfileEnable)
    {
        struct timeval tm;
        gettimeofday(&tm, NULL);

        for(i = 0; i < ARCNN_MAX_PROFILE; i++)
        {
            if(astProfile[i].strName == pchName)
                break;
        }
        if(i == ARCNN_MAX_PROFILE)
        {
            Log(ARCNN_LOG_ERR, "Profile has run out.\n");
            return 0;
        }

        astProfile[i].u64Toc = tm.tv_sec * 1000 + tm.tv_usec / 1000;
        printf("[ARCNN][PROF]%s, line %d: %s takes %lld ms\n",
            __FUNCTION__, __LINE__, astProfile[i].strName.c_str(), astProfile[i].u64Toc - astProfile[i].u64Tic);

        astProfile[i].strName.clear();
        astProfile[i].u64Tic = 0;
        astProfile[i].u64Toc = 0;
    }

    return 0;
}

AR_S32 CARMTCNN::EnableNPUDebug(AR_U32 u32Start, AR_U32 u32End)
{
	AR_S32 s32Ret = 0;
	s32Ret = AR_MPI_NPU_EnableNpuDebug(handle, u32Start, u32End, (AR_CHAR *)"/tmp/");
    return s32Ret;
}

AR_S32 CARMTCNN::DisableNPUDebug()
{
   	AR_S32 s32Ret = 0;
	s32Ret = AR_MPI_NPU_DisableNpuDebug(handle);
    return s32Ret;
}

AR_S32 CARMTCNN::RegisterLayerDebugFunc(AR_NPU_LayerDebugFunc pDebugFunc)
{
    return AR_MPI_NPU_RegisterLayerDebugfunc(handle, pDebugFunc);
}

AR_S32 CARMTCNN::RegisterCallbackFunc(AR_CHAR * chName, AR_NPU_CallbackFunc pCBFunc)
{
    return AR_MPI_NPU_RegisterCallback(handle, chName, pCBFunc);
}

AR_S32 CARMTCNN::RegisterIFCDebugFunc(AR_NPU_IFCDebugFunc pCBFunc)
{
    //debug temp
    return AR_MPI_NPU_RegisterIFCDebugfunc(handle, pCBFunc, (AR_CHAR *)"/mnt/");
}

AR_U32 CARMTCNN::IsInitialized()
{
    return u32Initialized;
}

AR_U32 CARMTCNN::GetInputNum()
{
    return AR_MPI_NPU_GetInputTensorNum(handle);
}

AR_S32 CARMTCNN::GetRuntimeSize()
{
    AR_S32 ret =0;
    AR_U32 u32Size;
    ret = AR_MPI_NPU_GetRuntimeSize(handle,&u32Size);
	if(ret<0)
		return ret;
	return u32Size;

}

AR_S32 CARMTCNN::SetRuntimeBuffer(AR_UINTPTR uptrVirt, AR_U64 u64Phy)
{
	return AR_MPI_NPU_SetRuntimeBuffer(handle, uptrVirt, u64Phy);
}

AR_S32 CARMTCNN::MallocNPUBuff()
{
    AR_U32 u32Size = 0;
    AR_S32 s32Ret = 0;

    u32Size = AR_MPI_NPU_GetInputBuffSize(handle);
    if(!u32Size)
    {
        Log(ARCNN_LOG_ERR, "Get NPU input size error.\n");
        return -1;
    }
	stNPUInBuff.u64Len = u32Size;
    s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"input", &stNPUInBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Malloc NPU input error: size %d.\n", u32Size);
        return -1;
    }

    u32Size = AR_MPI_NPU_GetOutputBuffSize(handle);
    if(!u32Size)
    {
        Log(ARCNN_LOG_ERR, "Get NPU output size error.\n");
        AR_MPI_NPU_FreeBuff(&stNPUInBuff);
        return -1;
    }

	stNPUOutBuff.u64Len = u32Size;
    s32Ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"output", &stNPUOutBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Malloc NPU output error: size %d.\n", u32Size);
        AR_MPI_NPU_FreeBuff(&stNPUInBuff);
        return -1;
    }
	memset((void*)stNPUInBuff.u64VirtAddr,0,stNPUInBuff.u64Len);
	memset((void*)stNPUOutBuff.u64VirtAddr,0,stNPUOutBuff.u64Len);
	AR_MPI_NPU_InvalidCachedBuff(&stNPUOutBuff);

    return 0;
}

AR_S32 CARMTCNN::FreeNPUBuff()
{
    AR_S32 s32Ret = 0;
    s32Ret = AR_MPI_NPU_FreeBuff(&stNPUInBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Free NPU input buffer error.\n");
    }

    s32Ret = AR_MPI_NPU_FreeBuff(&stNPUOutBuff);
    if(s32Ret)
    {
        Log(ARCNN_LOG_ERR, "Free NPU output buffer error.\n");
    }

    return s32Ret;
}


CARFaceDetector::CARFaceDetector(AR_CHAR * pchCfgFile)
{
    AR_CHAR * pchBuff = NULL;
    AR_U32 u32Size;
    AR_U32 u32LogLevelCtrl = 0;
    AR_S32 ret = 0;
	u32RuntimeMaxSize=0;

    AR_DOUBLE fPnetThresh = 0.0;
    AR_DOUBLE fRnetThresh = 0.0;
    AR_DOUBLE fOnetThresh = 0.0;

    u32Mode = 0;
    strLocalDir.clear();

    s32LiveFd = 0;
    //pstLiveImage = NULL;
    pchLocalImage = NULL;
    u32DrawOSD = 0;
    strDevName.clear();
    strOsdDevName.clear();

    fPnetNmsThresh = 0.0;
    fRnetNmsThresh = 0.0;
    fOnetNmsThresh = 0.0;
    listTotalBondingBox.clear();

    if(!pchCfgFile)
    {
        printf("Invalid json file name.\n");
        return;
    }

    ifstream cfgFile(pchCfgFile, ios::in|ios::binary|ios::ate);
    if(!cfgFile.is_open())
    {
        printf("Open json file failed.\n");
        return;
    }

    u32Size = cfgFile.tellg();
    cfgFile.seekg(0, ios::beg);
    pchBuff = new char[u32Size];
    if(!pchBuff)
    {
        printf("Get buffer error in constructor.\n");
        return;
    }

    cfgFile.read(pchBuff, u32Size);
    cfgFile.close();

    cJSON * pJson = NULL;
    cJSON * pNode = NULL;
    pJson = cJSON_Parse(pchBuff);
    if(!pJson)
    {
        printf("Parse cfg json file[%s] failed.\n", pchCfgFile);
        delete[] pchBuff;
        return;
    }

    pNode = cJSON_GetObjectItemCaseSensitive(pJson, "PNet_num");
    if(!pNode)
    {
        printf("Parse pnet num node failed.\n");
        delete[] pchBuff;
        cJSON_Delete(pJson);
        return ;
    }

    u32PnetNum = atoi(pNode->valuestring);
    if(u32PnetNum > 12)
    {
        printf("Max 12 pnet needed, but you configured %d.\n", u32PnetNum);
        delete[] pchBuff;
        cJSON_Delete(pJson);
        return;
    }

    char name[128] = {0};
    for(AR_U32 i = 0; i < u32PnetNum; i++)
    {
        sprintf(name, "PNet%d", i);
        pNode = cJSON_GetObjectItemCaseSensitive(pJson, name);
        if(!pNode)
        {
            printf("No pnet found.\n");
            delete[] pchBuff;
            cJSON_Delete(pJson);
            return;
        }

        ret = mPnet[i].Initialize(pNode->valuestring);
        if(ret)
        {
            printf("Init %s failed: %d.\n", name, ret);
            delete[] pchBuff;
            cJSON_Delete(pJson);
            return;
        }
    }

    pNode = cJSON_GetObjectItemCaseSensitive(pJson, "PNet_thresh");
    if(!pNode)
    {
        fPnetThresh = 0.5;
    }
    else
    {
        fPnetThresh = pNode->valuedouble;
    }

    for(AR_U32 i = 0; i < u32PnetNum; i++)
    {
        mPnet[i].SetThreshold(fPnetThresh);
    }

    pNode = cJSON_GetObjectItemCaseSensitive(pJson, "PNet_NMS_thresh");
    if(!pNode)
    {
        fPnetNmsThresh = 0.5;
    }
    else
    {
        fPnetNmsThresh = pNode->valuedouble;
    }

    pNode = cJSON_GetObjectItemCaseSensitive(pJson, "RNet");
    if(!pNode)
    {
        printf("No rnet found.\n");
        delete[] pchBuff;
        cJSON_Delete(pJson);
        return;
    }
    ret = mRnet.Initialize(pNode->valuestring);
    if(ret)
    {
        printf("Init rnet failed.\n");
        delete[] pchBuff;
        cJSON_Delete(pJson);
        return;
    }
    pNode = cJSON_GetObjectItemCaseSensitive(pJson, "RNet_thresh");
    if(!pNode)
    {
        fRnetThresh = 0.5;
    }
    else
    {
        fRnetThresh = pNode->valuedouble;
    }
    mRnet.SetThreshold(fRnetThresh);

    pNode = cJSON_GetObjectItemCaseSensitive(pJson, "RNet_NMS_thresh");
    if(!pNode)
    {
        fRnetNmsThresh = 0.5;
    }
    else
    {
        fRnetNmsThresh = pNode->valuedouble;
    }

    pNode = cJSON_GetObjectItemCaseSensitive(pJson, "ONet");
    if(!pNode)
    {
        printf("No onet found.\n");
        delete[] pchBuff;
        cJSON_Delete(pJson);
        return;
    }
    ret = mOnet.Initialize(pNode->valuestring);
    if(ret)
    {
        printf("Init onet failed.\n");
        delete[] pchBuff;
        cJSON_Delete(pJson);
        return;
    }
    pNode = cJSON_GetObjectItemCaseSensitive(pJson, "ONet_thresh");
    if(!pNode)
    {
        fOnetThresh = 0.5;
    }
    else
    {
        fOnetThresh = pNode->valuedouble;
    }
    mOnet.SetThreshold(fOnetThresh);

    pNode = cJSON_GetObjectItemCaseSensitive(pJson, "ONet_NMS_thresh");
    if(!pNode)
    {
        fOnetNmsThresh = 0.5;
    }
    else
    {
        fOnetNmsThresh = pNode->valuedouble;
    }

    pNode = cJSON_GetObjectItemCaseSensitive(pJson, "mode");
    if(!pNode)
    {
        printf("Parse mode node failed.\n");
        delete[] pchBuff;
        cJSON_Delete(pJson);
        return;
    }
    if(strcmp(pNode->valuestring, "local") == 0)
    {
        u32Mode = ARCNN_INPUT_LOCAL;
        cJSON * path = cJSON_GetObjectItemCaseSensitive(pJson, "local_path");
        if(!path)
        {
            printf("Parse local path failed.\n");
            delete[] pchBuff;
            cJSON_Delete(pJson);
            return;
        }

        strLocalDir = path->valuestring;
    }
    else
    {
        u32Mode = ARCNN_INPUT_LIVE;
        cJSON * devname = cJSON_GetObjectItemCaseSensitive(pJson, "devname");
        if(!devname)
        {
            printf("Parse devname failed.\n");
            delete[] pchBuff;
            cJSON_Delete(pJson);
            return;
        }

        strDevName = devname->valuestring;

        cJSON * drawosd = cJSON_GetObjectItemCaseSensitive(pJson, "draw_osd");
        if(!drawosd)
        {
            printf("Parse drawosd failed.\n");
            u32DrawOSD = 0;
        }
        else
        {
            if(strcmp(drawosd->valuestring, "yes") == 0)
            {
                u32DrawOSD = 1;
            }
            else
            {
                u32DrawOSD = 0;
            }
        }

        if(u32DrawOSD)
        {
            cJSON * osd = cJSON_GetObjectItemCaseSensitive(pJson, "osd_devname");
            if(!osd)
            {
                printf("No osd dev found.\n");
                delete[] pchBuff;
                cJSON_Delete(pJson);
                return;
            }

            strOsdDevName = osd->valuestring;
        }
    }

    pNode = cJSON_GetObjectItemCaseSensitive(pJson, "log_level");
    if(!pNode)
    {
        u32LogLevelCtrl = ARCNN_LOG_ERR;
    }
    else
    {
        u32LogLevelCtrl = atoi(pNode->valuestring);
    }

    mRnet.SetLogLevel(u32LogLevelCtrl);
    mOnet.SetLogLevel(u32LogLevelCtrl);
    for(AR_U32 i = 0; i < u32PnetNum; i++)
    {
        mPnet[i].SetLogLevel(u32LogLevelCtrl);
    }

    delete[] pchBuff;
    cJSON_Delete(pJson);

    // find Max Runtime Size
    u32RuntimeMaxSize = u32RuntimeMaxSize>mRnet.GetRuntimeSize()? u32RuntimeMaxSize:mRnet.GetRuntimeSize();
	u32RuntimeMaxSize = u32RuntimeMaxSize>mOnet.GetRuntimeSize()? u32RuntimeMaxSize:mOnet.GetRuntimeSize();
    for(AR_U32 i = 0; i < u32PnetNum; i++)
    {
		u32RuntimeMaxSize = u32RuntimeMaxSize>mPnet[i].GetRuntimeSize()? u32RuntimeMaxSize:mPnet[i].GetRuntimeSize();
    }
	if(u32RuntimeMaxSize<=0)
	{
		return;
	}
	printf("MaxSRuntimeSize = %d \r\n",u32RuntimeMaxSize);
	stRuntimeSharedBuff.u64Len = u32RuntimeMaxSize;

    ret = AR_MPI_NPU_MallocBuff((AR_CHAR *)"runtime", &stRuntimeSharedBuff);
    if(ret)
    {
        printf("Malloc NPU input error: size %d.\n", u32RuntimeMaxSize);
        return;
    }
    printf("SharedRuntime virt %llx phy %llx\r\n",stRuntimeSharedBuff.u64VirtAddr,stRuntimeSharedBuff.u64PhyAddr);

	AR_UINTPTR uptrVirt = (AR_UINTPTR)stRuntimeSharedBuff.u64VirtAddr ;
	AR_U64 u64Phy = (AR_U64)stRuntimeSharedBuff.u64PhyAddr;
	ret = mRnet.SetRuntimeBuffer(uptrVirt,u64Phy);
	ret = mOnet.SetRuntimeBuffer(uptrVirt,u64Phy);
	for(AR_U32 i = 0; i < u32PnetNum; i++)
    {
		ret = mPnet[i].SetRuntimeBuffer(uptrVirt,u64Phy);
    }
	if(ret < 0)
	{
	    printf("SetRuntimeBuffer error.\n");
		return;
	}
    u32Initialized = 1;
}

AR_U32 CARFaceDetector::GetMode()
{
    return u32Mode;
}

AR_S32 CARFaceDetector::BindVideoDev()
{
#if 0
    s32LiveFd = ar_alg_open_video_stream((char *)strDevName.c_str());
    if(!s32LiveFd)
    {
        printf("Open dev %s error.\n", strDevName.c_str());
        return -1;
    }
#endif
    return 0;
}

AR_S32 CARFaceDetector::GetLiveImage(AR_IMG_S * pstImg)
{
    //get img and translate to AR_IMG_S
    if(!s32LiveFd)
    {
        printf("Please BindVideoDev first.\n");
        return -1;
    }
#if 0
    pstLiveImage = ar_alg_get_img(s32LiveFd);
    if(!pstLiveImage)
    {
        printf("Get img error!\n");
        return -1;
    }

    pstImg->u16FrameId = (AR_U16)(pstLiveImage->frame_id & 0x0000FFFF);
    pstImg->u32Type = AR_IMG_YUV;
    pstImg->stYUV.u16Width = pstLiveImage->format.width;
    pstImg->stYUV.u16Height = pstLiveImage->format.height;
    pstImg->stYUV.u16Format = pstLiveImage->format.format;
    pstImg->stYUV.u16YStride = pstLiveImage->format.luma_stride;
    pstImg->stYUV.u16UStride = pstLiveImage->format.chroma_stride;
    pstImg->stYUV.u16VStride = pstLiveImage->format.chroma_stride;
    pstImg->stYUV.u32YAddrPhy = pstLiveImage->mem[0].pa;
    pstImg->stYUV.u32UAddrPhy = pstLiveImage->mem[1].pa;
    pstImg->stYUV.u32VAddrPhy = pstLiveImage->mem[2].pa;
#endif
    return 0;
}

AR_S32 CARFaceDetector::ReleaseLiveImage(AR_IMG_S * pstImg)
{
#if 0

    if(!s32LiveFd || !pstLiveImage)
    {
        printf("No video dev or image.\n");
        return -1;
    }

    //Compare pa to see if the buffer is the same one
    if(pstImg->stYUV.u32YAddrPhy == pstLiveImage->mem[0].pa
      && pstImg->stYUV.u32UAddrPhy == pstLiveImage->mem[1].pa
      && pstImg->stYUV.u32VAddrPhy == pstLiveImage->mem[2].pa)
    {
        ar_alg_release_img(s32LiveFd, pstLiveImage);
    }
    else
    {
        printf("The buffer is not the same one.\n");
        return -1;
    }
#endif
    return 0;
}

AR_S32 CARFaceDetector::DeBindVideoDev()
{
#if 0
    if(s32LiveFd)
    {
        ar_alg_close_video_stream(s32LiveFd);
    }
#endif
    return 0;
}

string CARFaceDetector::GetLocalTestDir()
{
    return strLocalDir;
}

//#define CALLBACK_DATA_LOAD
//For local image test. Need to add RGBD or RGBIntlv format later.
AR_S32 CARFaceDetector::GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg)
{
    AR_U32 u32FileType = 0;
    AR_S32 s32FileSize = 0;
    void * pchMMZBuffVirt = NULL;
    AR_U64 pchMMZBuffPhy = 0;
    static AR_U16 u16FrameId = 0;

    u32FileType = GetImageFileType(pchFileName);
    if(u32FileType == RGB_RAW_DATA_FILE || u32FileType == BGR_RAW_DATA_FILE)
    {
        ifstream file(pchFileName, ios::in|ios::binary|ios::ate);
        if(file.is_open())
        {
            s32FileSize = file.tellg();
            file.seekg(0, ios::beg);
            GetMMZMemory(&pchMMZBuffPhy, &pchMMZBuffVirt, (AR_CHAR *)"Pchbuff", s32FileSize);
            if((pchMMZBuffPhy == 0) || !pchMMZBuffVirt)
            {
                printf("Malloc MMZ failed.\n");
                return -1;
            }

            file.read((AR_CHAR *)pchMMZBuffVirt, s32FileSize);
            file.close();

            pstImg->u32FrameId = u16FrameId++;
            pstImg->enFormat = AR_IMG_RGB;
            //No need to set w/h/s since ifc.json will set these parameters.
            pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)pchMMZBuffPhy;
            pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchMMZBuffVirt;
        }
    }
    else if(u32FileType == JPG_BMP_PNG_IMG_FILE)
    {
        //printf("Not implemented yet.\n");
        #if 1
        //read image and resize it, TBD
        cv::Mat src_img = cv::imread(pchFileName);
        if(src_img.empty())
        {
           printf("Imread file %s failed!\r\n", pchFileName);
           return -1;
        }

        s32FileSize = src_img.cols * src_img.rows * src_img.channels();
        GetMMZMemory(&pchMMZBuffPhy, &pchMMZBuffVirt, (AR_CHAR *)"Pchbuff", s32FileSize);
        if((pchMMZBuffPhy == 0) || !pchMMZBuffVirt)
        {
            printf("Malloc MMZ failed.\n");
            return -1;
        }

        cv::Mat channels[3];
        cv::split(src_img, channels);
        AR_U32 u32ChSize = src_img.cols * src_img.rows;

        memcpy(pchMMZBuffVirt, channels[2].data, u32ChSize);
        memcpy((AR_CHAR *)pchMMZBuffVirt + u32ChSize, channels[1].data, u32ChSize);
        memcpy((AR_CHAR *)pchMMZBuffVirt + u32ChSize * 2, channels[0].data, u32ChSize);

        pstImg->u32FrameId = u16FrameId++;
        pstImg->enFormat = AR_IMG_RGB;
        pstImg->u32ChannelNum = 3;
        pstImg->u32Width = src_img.cols;
        pstImg->u32Height = src_img.rows;

        pstImg->astChannels[0].u32Stride = src_img.cols;
        pstImg->astChannels[0].u32AddrPhy = (AR_UINTPTR)pchMMZBuffPhy;
        pstImg->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pchMMZBuffVirt;

        pstImg->astChannels[1].u32Stride = src_img.cols;
        pstImg->astChannels[1].u32AddrPhy = (AR_UINTPTR)(pchMMZBuffPhy + u32ChSize);
        pstImg->astChannels[1].uptrAddrVirt = (AR_UINTPTR)((AR_CHAR *)pchMMZBuffVirt + u32ChSize);

        pstImg->astChannels[2].u32Stride = src_img.cols;
        pstImg->astChannels[2].u32AddrPhy = (AR_UINTPTR)(pchMMZBuffPhy + 2 * u32ChSize);
        pstImg->astChannels[2].uptrAddrVirt = (AR_UINTPTR)((AR_CHAR *)pchMMZBuffVirt + 2 * u32ChSize);

        pstImg->astChannels[3].u32AddrPhy = 0;
        pstImg->astChannels[3].uptrAddrVirt = 0;

        //SaveRGBImg(pchIONBuffVirt, pchIONBuffVirt + u32ChSize, pchIONBuffVirt + u32ChSize *2, pstImg->stRGB.u16Stride, pstImg->u32Height, "/mnt/src.jpg");
        printf("Get local img: %s w %d h %d s %d/%d/%d\n", pchFileName,
            pstImg->u32Width, pstImg->u32Height, pstImg->astChannels[0].u32Stride, pstImg->astChannels[1].u32Stride, pstImg->astChannels[2].u32Stride);
        #endif
    }
    else
    {
        //Log(ARCNN_LOG_ERR, "Invalid file type.\n");
        return -1;
    }

    return 0;
}

AR_S32 CARFaceDetector::ReleaseLocalImage(AR_IMG_S * pstImg)
{
    //Only need to release RGB data, YUV is released by other apis.
    if( (pstImg->enFormat == AR_IMG_RGB || pstImg->enFormat == AR_IMG_RGBD)
         && pstImg->astChannels[0].uptrAddrVirt )
    {
        //AR_RELEASE_ION_MEMORY(pstImg->stRGB.u32RAddrVirt);
        ReleaseMMZMemory((AR_U64)pstImg->astChannels[0].u32AddrPhy, (void *)pstImg->astChannels[0].uptrAddrVirt);
             pstImg->astChannels[0].uptrAddrVirt = 0;
    }

    return 0;
}

#define MAX_BONDINGBOX_NUM     16384 //temp max value
#define RESIZE_BUFFER_SIZE    0x1000000 //16MB to save cropped img

AR_S32 CARFaceDetector::Process(AR_IMG_S * pstImg, AR_BOOL bInstant, AR_BOOL bDebug)
{
    //AR_U32 i = 0;
    AR_U32 s32Ret = 0;
    char tmp[128] = {0};

    BondingBoxList::iterator it;

    if(pstImg->enFormat != AR_IMG_RGB)
    {
        printf("Live mode for YUV not ready yet.\n");
        return -1;
    }

    listTotalBondingBox.clear();

    s32Ret = PnetProcess(pstImg);
    if(s32Ret)
    {
        printf("Run Pnet error.\n");
        return -1;
    }

    //totally nms
    listTotalBondingBox.sort(CompareScore);
    CalcNMS(listTotalBondingBox, 0.7, ARMTCNN_NMS_UNION);

    /////////debug
    sprintf(tmp, "./pnet_total_nms.txt");
    ofstream dbgp(tmp);
    for(it = listTotalBondingBox.begin(); it != listTotalBondingBox.end(); it++)
    {
        dbgp <<"w,h: " << it->u32IndexW -1 << " " << it->u32IndexH -1 << "Score " << it->dScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<"]"
             << " [ " << it->fDx0 << " " << it->fDy0 << " " << it->fDx1 << " " << it->fDy1 <<"]" << endl;
    }
    //////////

    printf("Total %ld bbox after Pnet.\n", listTotalBondingBox.size());

    if(listTotalBondingBox.size() == 0)
    {
        return 0;
    }

    s32Ret = RnetProcess(pstImg);
    if(s32Ret)
    {
        printf("Run Rnet error.\n");
        return -1;
    }

    listTotalBondingBox.sort(CompareScore);
    CalcNMS(listTotalBondingBox, fRnetNmsThresh, ARMTCNN_NMS_UNION);
    printf("Total %ld bbox after Rnet.\n", listTotalBondingBox.size());

    sprintf(tmp, "/mnt/rnet_total_nms.txt");
    ofstream dbgr(tmp);
    for(it = listTotalBondingBox.begin(); it != listTotalBondingBox.end(); it++)
    {
        dbgp <<"w,h: " << it->u32IndexW -1 << " " << it->u32IndexH -1 << "Score " << it->dScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<"]"
             << " [ " << it->fDx0 << " " << it->fDy0 << " " << it->fDx1 << " " << it->fDy1 <<"]" << endl;
    }

    if(listTotalBondingBox.size() == 0)
    {
        return 0;
    }

    s32Ret = OnetProcess(pstImg);
    if(s32Ret)
    {
        printf("Run Onet error.\n");
        return -1;
    }

    listTotalBondingBox.sort(CompareScore);
    CalcNMS(listTotalBondingBox, fOnetNmsThresh, ARMTCNN_NMS_MIN);

    sprintf(tmp, "/mnt/onet_total_nms.txt");
    ofstream dbgo(tmp);
    for(it = listTotalBondingBox.begin(); it != listTotalBondingBox.end(); it++)
    {
        dbgo <<"w,h: " << it->u32IndexW -1 << " " << it->u32IndexH -1 << "Score " << it->dScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<"]"
             << " [ " << it->fDx0 << " " << it->fDy0 << " " << it->fDx1 << " " << it->fDy1 <<"]" << endl;
    }

    printf("-------[%ld]-Result---------\n", listTotalBondingBox.size());
    for(it = listTotalBondingBox.begin(); it != listTotalBondingBox.end(); it++)
    {
        if(it->u32Valid)
        {
            printf("Box[%d %d]: %f [%f %f %f %f]\n", it->u32IndexW, it->u32IndexH, it->dScore, it->fX0, it->fY0, it->fX1, it->fY1);
        }
    }
    printf("---------------------------\n");

    return 0;
}

AR_S32 CARFaceDetector::PnetProcess(AR_IMG_S * pstImg)
{
    AR_U32 u32PnetRealNum = 0; //pyramid levels
    void * pDstAddrVirt = 0;
    AR_U64 pDstAddrPhy = 0;
    AR_CHAR * pRszBuffVirt = 0;
    AR_CHAR * pRszBuffPhy = 0;
    AR_U32 i = 0;
    AR_S32 s32Ret = 0;
    AR_U32 u32BoxCnt = 0;
    //char tmp[512] = {0};

	AR_IMG_SET_S *pstImgSet =(AR_IMG_SET_S *)malloc(sizeof(AR_IMG_SET_S));
    if(!pstImgSet)
    {
        printf("No mem!\n");
        return -1;
    }

    //wrapper to store imgs
    AR_IMG_S * astDstImg = (AR_IMG_S *)malloc(sizeof(AR_IMG_S) * MAX_BONDINGBOX_NUM);
    if(!astDstImg)
    {
        printf("No mem!\n");
		free(pstImgSet);
        return -1;
    }
    memset(astDstImg, 0, sizeof(AR_IMG_S) * MAX_BONDINGBOX_NUM);

    AR_HAL_SCALER_CROP_S * astCrop = (AR_HAL_SCALER_CROP_S *)malloc(sizeof(AR_HAL_SCALER_CROP_S) * MAX_BONDINGBOX_NUM);
    if(!astCrop)
    {
        printf("No mem!\n");
		free(pstImgSet);
        free(astDstImg);
        return -1;
    }
    memset(astCrop, 0, sizeof(AR_HAL_SCALER_CROP_S) * MAX_BONDINGBOX_NUM);

    //AR_GET_ION_MEMORY(pDstAddrVirt, pDstAddrPhy, RESIZE_BUFFER_SIZE, 0);
    GetMMZMemory(&pDstAddrPhy, &pDstAddrVirt, (AR_CHAR *)"PNet", RESIZE_BUFFER_SIZE);
    if(!pDstAddrVirt)
    {
        printf("Malloc ION failed.\n");
		free(pstImgSet);
        free(astDstImg);
        free(astCrop);
        return -1;
    }

    //Do pyramid resize.
    i = ARMTCNN_PNET_START_INDEX;
    pRszBuffVirt = (AR_CHAR*)pDstAddrVirt;
    pRszBuffPhy = (AR_CHAR*)pDstAddrPhy;

    while(1)
    {
        AR_DOUBLE dScale = (ARMTCNN_CELLSIZE / ARMTCNN_MIN_FACE_SIZE) * pow(ARMTCNN_PYRAMID_FACTOR, i);
        AR_U16 u16Width = ceil(pstImg->u32Width * dScale);
        AR_U16 u16Height = ceil(pstImg->u32Height * dScale);
        AR_U16 u16Stride = ALIGNED_256B(u16Width);

        if(u16Width < ARMTCNN_CELLSIZE || u16Height < ARMTCNN_CELLSIZE)
        {
            u32PnetRealNum = i;
            break;
        }

        printf("PNET %d: %d x %d, scale: %f\n", i, u16Width, u16Height, dScale);
        mPnet[i].SetScaleParam(dScale, u16Width, u16Height, pstImg->u32Width, pstImg->u32Height);

        astDstImg[i].enFormat = AR_IMG_RGB;
        astDstImg[i].u32Height = u16Height;
        astDstImg[i].u32Width = u16Width;
        astDstImg[i].u32ChannelNum = 3;

        astDstImg[i].astChannels[0].u32Stride = u16Stride;
        astDstImg[i].astChannels[0].u32AddrPhy = (AR_UINTPTR)pRszBuffPhy;
        astDstImg[i].astChannels[0].uptrAddrVirt = (AR_UINTPTR)pRszBuffVirt;
        astDstImg[i].astChannels[1].u32Stride = u16Stride;
        astDstImg[i].astChannels[1].u32AddrPhy = (AR_UINTPTR)(pRszBuffPhy + u16Stride * u16Height);
        astDstImg[i].astChannels[1].uptrAddrVirt = (AR_UINTPTR)(pRszBuffVirt + u16Stride * u16Height);
        astDstImg[i].astChannels[2].u32Stride = u16Stride;
        astDstImg[i].astChannels[2].u32AddrPhy = (AR_UINTPTR)(pRszBuffPhy + u16Stride * u16Height * 2);
        astDstImg[i].astChannels[2].uptrAddrVirt = (AR_UINTPTR)(pRszBuffVirt + u16Stride * u16Height * 2);

#ifdef AR_SCALER_SINGLE_MODE
    #ifdef SCALER_MATLAB_TEST
        //load test img
        cv::Mat img = cv::imread("/mnt/matlab615x461.bmp");
        cv::Mat channels[3];
        cv::split(img, channels);

        for(int h = 0; h < u16Height; h++)
        {
            memcpy((AR_CHAR *)(astDstImg[i].astChannels[0].uptrAddrVirt + h * u16Stride), channels[2].data + h * u16Width, u16Width);
            memcpy((AR_CHAR *)(astDstImg[i].astChannels[0].uptrAddrVirt + u16Stride * u16Height + h * u16Stride), channels[1].data + h * u16Width, u16Width);
            memcpy((AR_CHAR *)(astDstImg[i].astChannels[0].uptrAddrVirt + u16Stride * u16Height * 2 + h * u16Stride), channels[0].data + h * u16Width, u16Width);
        }

        i++;
        u32PnetRealNum++;

        break;
    #else
        AR_HAL_SCALER_CROP_S stCrop;
        stCrop.u32X = 0;
        stCrop.u32Y = 0;
        stCrop.u32W = pstImg->u32Width;
        stCrop.u32H = pstImg->u32Height;
        s32Ret = CropAndResize(pstImg, &stCrop, &astDstImg[i], 1);
        if(s32Ret < 0)
        {
            printf("Do crop/resize error.\n");
            break;
        }
    #endif
#else
        astCrop[i].u32X = 0;
        astCrop[i].u32Y = 0;
        astCrop[i].u32W = pstImg->u32Width;
        astCrop[i].u32H = pstImg->u32Height;
#endif
        pRszBuffVirt += u16Stride * u16Height * 3;
        pRszBuffPhy += u16Stride * u16Height * 3;

#ifdef AR_SCALER_DEBUG
        AR_CHAR tmp[128] = {0};
        sprintf(tmp, "/mnt/%dx%d.bmp", u16Height, u16Width);
        SaveRGBImg((AR_CHAR *)astDstImg[i].astChannels[0].u32AddrVirt,
            (AR_CHAR *)(astDstImg[i].astChannels[0].uptrAddrVirt + u16Stride * u16Height),
            (AR_CHAR *)(astDstImg[i].astChannels[0].uptrAddrVirt + u16Stride * u16Height * 2), u16Stride, u16Height, tmp);

#endif
        i++;
    }


    if(u32PnetRealNum == 0)
    {
        printf("No pyramid, exit.\n");
		free(pstImgSet);
        free(astDstImg);
		free(astCrop);
        //AR_RELEASE_ION_MEMORY(pDstAddrVirt);
        ReleaseMMZMemory((AR_U64)pDstAddrPhy, (void *)pDstAddrVirt);
        return -1;
    }

    //batch mode, crop/resize max batch is 255
#ifndef AR_SCALER_SINGLE_MODE
    AR_U32 u32CropLoop, u32CropLeft;
    u32CropLoop = u32PnetRealNum / AR_SCALER_MAX_BATCH;
    u32CropLeft = u32PnetRealNum % AR_SCALER_MAX_BATCH;
    for(i = 0; i < u32CropLoop; i++)
    {
        CropAndResize(pstImg, astCrop, astDstImg, AR_SCALER_MAX_BATCH);
    }
    if(u32CropLeft)
    {
        CropAndResize(pstImg, astCrop + u32CropLoop * AR_SCALER_MAX_BATCH, astDstImg + u32CropLoop * AR_SCALER_MAX_BATCH, u32CropLeft);
    }
#endif

    //Pnet 0 - 11, should every pnet do nms with its output first ?
    for(i = ARMTCNN_PNET_START_INDEX; i < u32PnetRealNum; i++)
    {
        memset(pstImgSet, 0, sizeof(AR_IMG_SET_S));
		memcpy(&pstImgSet->astInputImg[0].astBatchImg[0],&astDstImg[i],sizeof(AR_IMG_S));
		if(mPnet[i].GetIfcPreProcess())
			pstImgSet->astInputImg[0].bPreIfcProcess =AR_TRUE;

		s32Ret = mPnet[i].Forward(pstImgSet, AR_TRUE, AR_FALSE);
        if(s32Ret)
        {
            printf("Pnet %d forward failed.\n", i);
			free(pstImgSet);
            free(astDstImg);
            free(astCrop);
            //AR_RELEASE_ION_MEMORY(pDstAddrVirt);
            ReleaseMMZMemory((AR_U64)pDstAddrPhy, (void *)pDstAddrVirt);
            return -1;
        }

        mPnet[i].PostProcess(ARMTCNN_POST_MODE_PNET);
        printf("Pnet%d got %d bbox.\n", i, mPnet[i].GetBondingBoxNum());

        mPnet[i].Nms(fPnetNmsThresh);//do nms after each pnet

        u32BoxCnt = mPnet[i].GetBondingBoxNum();
        printf("Pnet%d got %d bbox after NMS.\n", i, u32BoxCnt);

        for(AR_U32 j = 0; j < u32BoxCnt; j++)
        {
            listTotalBondingBox.push_back(mPnet[i].GetBondingBox());
        }

        //clear the img buffer
        memset((AR_CHAR *)astDstImg[i].astChannels[0].uptrAddrVirt, 0, astDstImg[i].astChannels[0].u32Stride * astDstImg[i].u32Height * 3);
    }

    //AR_RELEASE_ION_MEMORY(pDstAddrVirt);
    ReleaseMMZMemory((AR_U64)pDstAddrPhy, (void *)pDstAddrVirt);
	free(pstImgSet);
    free(astDstImg);
	free(astCrop);

    return 0;
}

AR_S32 CARFaceDetector::RnetProcess(AR_IMG_S * pstImg)
{
    AR_U32 u32CropCnt = 0;
    AR_U32 u32Batch = 0;
    AR_U32 u32LoopCnt = 0;
    AR_U32 u32Left = 0;
    void * pDstAddrVirt = 0;
    AR_U64  pDstAddrPhy = 0;
    AR_CHAR * pRszBuffVirt = 0;
    AR_CHAR * pRszBuffPhy = 0;
    AR_U32 i = 0;
    AR_S32 s32Ret = 0;
    AR_U32 u32BoxCnt = 0;

    BondingBoxList::iterator it;

	AR_IMG_SET_S *pstImgSet =(AR_IMG_SET_S *)malloc(sizeof(AR_IMG_SET_S));
    if(!pstImgSet)
    {
        printf("No mem!\n");
        return -1;
    }

    //wrapper to store imgs
    AR_IMG_S * astDstImg = (AR_IMG_S *)malloc(sizeof(AR_IMG_S) * MAX_BONDINGBOX_NUM);
    if(!astDstImg)
    {
        printf("No mem!\n");
		free(pstImgSet);
        return -1;
    }
    memset(astDstImg, 0, sizeof(AR_IMG_S) * MAX_BONDINGBOX_NUM);

    AR_HAL_SCALER_CROP_S * astCrop = (AR_HAL_SCALER_CROP_S *)malloc(sizeof(AR_HAL_SCALER_CROP_S) * MAX_BONDINGBOX_NUM);
    if(!astCrop)
    {
        printf("No mem!\n");
		free(pstImgSet);
        free(astDstImg);
        return -1;
    }
    memset(astCrop, 0, sizeof(AR_HAL_SCALER_CROP_S) * MAX_BONDINGBOX_NUM);

    //AR_GET_ION_MEMORY(pDstAddrVirt, pDstAddrPhy, RESIZE_BUFFER_SIZE, 0);
    GetMMZMemory(&pDstAddrPhy, &pDstAddrVirt, (AR_CHAR *)"RNet", RESIZE_BUFFER_SIZE);
    if(!pDstAddrVirt)
    {
        printf("Malloc ION failed.\n");
		free(pstImgSet);
        free(astDstImg);
        free(astCrop);
        return -1;
    }

    printf("Start Rnet...\n");
    ofstream dbgb("./pnet_bboxreg.txt");
    ofstream dbgr("./pnet_rerec.txt");
    ofstream dbgd("./rnet_filter.txt");

    //do bbregression and rect->square, then crop
    i = 0;
    pRszBuffVirt = (AR_CHAR*)pDstAddrVirt;
    pRszBuffPhy = (AR_CHAR*)pDstAddrPhy;
    for(it = listTotalBondingBox.begin(); it != listTotalBondingBox.end(); it++)
    {
        BBoxRegression(it);

        dbgb <<"w,h: " << it->u32IndexW -1 << " " << it->u32IndexH -1 << "Score " << it->dScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<"]"
             << " [ " << it->fDx0 << " " << it->fDy0 << " " << it->fDx1 << " " << it->fDy1 <<"]" << endl;

        ReRectangle(it);

        dbgr <<"w,h: " << it->u32IndexW -1 << " " << it->u32IndexH -1 << "Score " << it->dScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<"]"
             << " [ " << it->fDx0 << " " << it->fDy0 << " " << it->fDx1 << " " << it->fDy1 <<"]" << endl;

        it->fX0 = int(CAP(it->fX0, 0, pstImg->u32Width - 1));
        it->fY0 = int(CAP(it->fY0, 0, pstImg->u32Height - 1));
        it->fX1 = int(CAP(it->fX1, 0, pstImg->u32Width - 1));
        it->fY1 = int(CAP(it->fY1, 0, pstImg->u32Height - 1));

        astDstImg[i].enFormat = AR_IMG_RGB;
        astDstImg[i].u32Height = ARMTCNN_RNET_HEIGHT;
        astDstImg[i].u32Width = ARMTCNN_RNET_WIDTH;
        astDstImg[i].u32ChannelNum = 3;

        astDstImg[i].astChannels[0].u32Stride = ALIGNED_256B(ARMTCNN_RNET_WIDTH);
        astDstImg[i].astChannels[0].u32AddrPhy = (AR_UINTPTR)pRszBuffPhy;
        astDstImg[i].astChannels[0].uptrAddrVirt = (AR_UINTPTR)pRszBuffVirt;
        astDstImg[i].astChannels[1].u32Stride = ALIGNED_256B(ARMTCNN_RNET_WIDTH);
        astDstImg[i].astChannels[1].u32AddrPhy = (AR_UINTPTR)(pRszBuffPhy + ARMTCNN_RNET_HEIGHT * ALIGNED_256B(ARMTCNN_RNET_WIDTH));
        astDstImg[i].astChannels[1].uptrAddrVirt = (AR_UINTPTR)(pRszBuffVirt + ARMTCNN_RNET_HEIGHT * ALIGNED_256B(ARMTCNN_RNET_WIDTH));
        astDstImg[i].astChannels[2].u32Stride = ALIGNED_256B(ARMTCNN_RNET_WIDTH);
        astDstImg[i].astChannels[2].u32AddrPhy = (AR_UINTPTR)(pRszBuffPhy + ARMTCNN_RNET_HEIGHT * ALIGNED_256B(ARMTCNN_RNET_WIDTH) * 2);
        astDstImg[i].astChannels[2].uptrAddrVirt = (AR_UINTPTR)(pRszBuffVirt + ARMTCNN_RNET_HEIGHT * ALIGNED_256B(ARMTCNN_RNET_WIDTH) * 2);

#ifdef AR_SCALER_SINGLE_MODE
        AR_HAL_SCALER_CROP_S stCrop;
        stCrop.u32X = int(it->fX0);
        stCrop.u32Y = int(it->fY0);
        stCrop.u32W = int(it->fX1 - it->fX0 + 1);
        stCrop.u32H = int(it->fY1 - it->fY0 + 1);

        s32Ret = CropAndResize(pstImg, &stCrop, &astDstImg[i], 1);
        if(s32Ret)
        {
            printf("Crop/resize error.\n");
            break;
        }
#else
        astCrop[i].u32X = int(it->fX0);
        astCrop[i].u32Y = int(it->fY0);
        astCrop[i].u32W = int(it->fX1 - it->fX0 + 1);
        astCrop[i].u32H = int(it->fY1 - it->fY0 + 1);
#endif

        pRszBuffVirt += ARMTCNN_RNET_HEIGHT * ALIGNED_256B(ARMTCNN_RNET_WIDTH) * 3;
        pRszBuffPhy += ARMTCNN_RNET_HEIGHT * ALIGNED_256B(ARMTCNN_RNET_WIDTH) * 3;

        i++;
    }

#ifndef AR_SCALER_SINGLE_MODE
    AR_U32 u32CropLoop, u32CropLeft;
    u32CropLoop = i / AR_SCALER_MAX_BATCH;
    u32CropLeft = i % AR_SCALER_MAX_BATCH;
    for(i = 0; i < u32CropLoop; i++)
    {
        CropAndResize(pstImg, astCrop, astDstImg, AR_SCALER_MAX_BATCH);
    }
    if(u32CropLeft)
    {
        CropAndResize(pstImg, astCrop + u32CropLoop * AR_SCALER_MAX_BATCH, astDstImg + u32CropLoop * AR_SCALER_MAX_BATCH, u32CropLeft);
    }
#endif

    //run batch rnet
    u32CropCnt = i;
    u32Batch = mRnet.GetBatchNum();
    u32LoopCnt = u32CropCnt / u32Batch;
    u32Left = u32CropCnt % u32Batch;
    it = listTotalBondingBox.begin();

    for(i = 0; i < u32LoopCnt; i++)
    {
		memset(pstImgSet, 0, sizeof(AR_IMG_SET_S));
		for(AR_U32 j = 0; j < u32Batch; j++)
    	{
			memcpy(&pstImgSet->astInputImg[0].astBatchImg[j],&astDstImg[i*u32Batch+j],sizeof(AR_IMG_S));
			if(mRnet.GetIfcPreProcess()){
				pstImgSet->astInputImg[0].bPreIfcProcess =AR_TRUE;
			}
		}

        mRnet.Forward(pstImgSet, AR_TRUE, AR_FALSE);
        mRnet.PostProcess(ARMTCNN_POST_MODE_RNET);

        u32BoxCnt = mRnet.GetBondingBoxNum();
        if(u32BoxCnt != u32Batch)
        {
            printf("Rnet: Mismatch batch and box cnt: %d %d.\n", u32BoxCnt, u32Batch);
            break;
        }

        for(AR_U32 j = 0; j < u32BoxCnt; j++)
        {
            BONDING_BOX_S stBox = mRnet.GetBondingBox();

            //dbgr << j + i*u32Batch << ": " << stBox.u32Valid <<": NewScore " << stBox.fScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<" ]" << endl;

            //drop the invalid box
            if(!stBox.u32Valid)
            {
                it->u32Valid = 0;
            }
            else
            {
                it->dScore = stBox.dScore;
                it->fDx0 = stBox.fDx0;
                it->fDy0 = stBox.fDy0;
                it->fDx1 = stBox.fDx1;
                it->fDy1 = stBox.fDy1;
            }

            it++;
        }
    }

    if(u32Left)
    {
    	memset(pstImgSet, 0, sizeof(AR_IMG_SET_S));
		for(AR_U32 j = 0; j < u32Left; j++)
    	{
			memcpy(&pstImgSet->astInputImg[0].astBatchImg[j],&astDstImg[u32LoopCnt*u32Batch+j],sizeof(AR_IMG_S));
			if(mRnet.GetIfcPreProcess()){
				pstImgSet->astInputImg[0].bPreIfcProcess =AR_TRUE;
			}
		}

        mRnet.Forward(pstImgSet, AR_TRUE, AR_FALSE);
        mRnet.PostProcess(ARMTCNN_POST_MODE_RNET);

        for(AR_U32 j = 0; j < u32Left; j++)
        {
            BONDING_BOX_S stBox = mRnet.GetBondingBox();

            //dbgr << j + u32Batch * u32LoopCnt << ": " << stBox.u32Valid << ": NewScore " << stBox.fScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<" ]" << endl;

            //drop the invalid box
            if(!stBox.u32Valid)
            {
                it->u32Valid = 0;
            }
            else
            {
                it->dScore = stBox.dScore;
                it->fDx0 = stBox.fDx0;
                it->fDy0 = stBox.fDy0;
                it->fDx1 = stBox.fDx1;
                it->fDy1 = stBox.fDy1;
            }

            it++;
        }
    }

    //delete invalid box, shrink the list
    it = listTotalBondingBox.begin();
    while(it != listTotalBondingBox.end())
    {
        if(!it->u32Valid)
        {
            it = listTotalBondingBox.erase(it);
        }
        else
        {
            dbgd <<"w,h: " << it->u32IndexW -1 << " " << it->u32IndexH -1 << "Score " << it->dScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<"]"
             << " [ " << it->fDx0 << " " << it->fDy0 << " " << it->fDx1 << " " << it->fDy1 <<"]" << endl;
            it++;
        }
    }

    //AR_RELEASE_ION_MEMORY(pDstAddrVirt);
    ReleaseMMZMemory((AR_U64)pDstAddrPhy, (void *)pDstAddrVirt);
	free(pstImgSet);
    free(astDstImg);
    free(astCrop);
    return 0;
}

AR_S32 CARFaceDetector::OnetProcess(AR_IMG_S * pstImg)
{
    AR_U32 u32CropCnt = 0;
    AR_U32 u32Batch = 0;
    AR_U32 u32LoopCnt = 0;
    AR_U32 u32Left = 0;
    void* pDstAddrVirt = 0;
    AR_U64 pDstAddrPhy = 0;
    AR_CHAR * pRszBuffVirt = 0;
    AR_U64 pRszBuffPhy = 0;
    AR_U32 i = 0;
    AR_S32 s32Ret = 0;
    AR_U32 u32BoxCnt = 0;

    BondingBoxList::iterator it;

	AR_IMG_SET_S *pstImgSet =(AR_IMG_SET_S *)malloc(sizeof(AR_IMG_SET_S));
    if(!pstImgSet)
    {
        printf("No mem!\n");
        return -1;
    }

    //wrapper to store imgs
    AR_IMG_S * astDstImg = (AR_IMG_S *)malloc(sizeof(AR_IMG_S) * MAX_BONDINGBOX_NUM);
    if(!astDstImg)
    {
        printf("No mem!\n");
		free(pstImgSet);
        return -1;
    }
    memset(astDstImg, 0, sizeof(AR_IMG_S) * MAX_BONDINGBOX_NUM);

    AR_HAL_SCALER_CROP_S * astCrop = (AR_HAL_SCALER_CROP_S *)malloc(sizeof(AR_HAL_SCALER_CROP_S) * MAX_BONDINGBOX_NUM);
    if(!astCrop)
    {
        printf("No mem!\n");
		free(pstImgSet);
        free(astDstImg);
        return -1;
    }
    memset(astCrop, 0, sizeof(AR_HAL_SCALER_CROP_S) * MAX_BONDINGBOX_NUM);

    //AR_GET_ION_MEMORY(pDstAddrVirt, pDstAddrPhy, RESIZE_BUFFER_SIZE, 0);
    GetMMZMemory(&pDstAddrPhy, &pDstAddrVirt, (AR_CHAR *)"Onet", RESIZE_BUFFER_SIZE);
    if(!pDstAddrVirt)
    {
        printf("Malloc ION failed.\n");
		free(pstImgSet);
        free(astDstImg);
        free(astCrop);
        return -1;
    }

    printf("Start Onet...\n");
    ofstream dbgb("/mnt/rnet_bboxreg.txt");
    ofstream dbgr("/mnt/rnet_rerec.txt");
    ofstream dbgd("/mnt/onet_filter.txt");
    ofstream dbgob("/mnt/onet_bboxreg.txt");

    i = 0;
    pRszBuffVirt = (AR_CHAR*)pDstAddrVirt;
    pRszBuffPhy = pDstAddrPhy;
    for(it = listTotalBondingBox.begin(); it != listTotalBondingBox.end(); it++)
    {
        BBoxRegression(it);

        dbgb <<"w,h: " << it->u32IndexW -1 << " " << it->u32IndexH -1 << "Score " << it->dScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<"]"
             << " [ " << it->fDx0 << " " << it->fDy0 << " " << it->fDx1 << " " << it->fDy1 <<"]" << endl;

        ReRectangle(it);

        dbgr <<"w,h: " << it->u32IndexW -1 << " " << it->u32IndexH -1 << "Score " << it->dScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<"]"
             << " [ " << it->fDx0 << " " << it->fDy0 << " " << it->fDx1 << " " << it->fDy1 <<"]" << endl;

        it->fX0 = int(CAP(it->fX0, 0, pstImg->u32Width - 1));
        it->fY0 = int(CAP(it->fY0, 0, pstImg->u32Height - 1));
        it->fX1 = int(CAP(it->fX1, 0, pstImg->u32Width - 1));
        it->fY1 = int(CAP(it->fY1, 0, pstImg->u32Height - 1));

        astDstImg[i].enFormat = AR_IMG_RGB;
        astDstImg[i].u32Height = ARMTCNN_ONET_HEIGHT;
        astDstImg[i].u32Width = ARMTCNN_ONET_WIDTH;
        astDstImg[i].u32ChannelNum = 3;


        astDstImg[i].astChannels[0].u32Stride = ALIGNED_256B(ARMTCNN_ONET_WIDTH);
        astDstImg[i].astChannels[0].u32AddrPhy = (AR_UINTPTR)pRszBuffPhy;
        astDstImg[i].astChannels[0].uptrAddrVirt = (AR_UINTPTR)pRszBuffVirt;
        astDstImg[i].astChannels[1].u32Stride = ALIGNED_256B(ARMTCNN_ONET_WIDTH);
        astDstImg[i].astChannels[1].u32AddrPhy = (AR_UINTPTR)(pRszBuffPhy + ARMTCNN_ONET_HEIGHT * ALIGNED_256B(ARMTCNN_ONET_WIDTH));
        astDstImg[i].astChannels[1].uptrAddrVirt = (AR_UINTPTR)(pRszBuffVirt + ARMTCNN_ONET_HEIGHT * ALIGNED_256B(ARMTCNN_ONET_WIDTH));
        astDstImg[i].astChannels[2].u32Stride = ALIGNED_256B(ARMTCNN_ONET_WIDTH);
        astDstImg[i].astChannels[2].u32AddrPhy = (AR_UINTPTR)(pRszBuffPhy + ARMTCNN_ONET_HEIGHT * ALIGNED_256B(ARMTCNN_ONET_WIDTH) * 2);
        astDstImg[i].astChannels[2].uptrAddrVirt = (AR_UINTPTR)(pRszBuffVirt + ARMTCNN_ONET_HEIGHT * ALIGNED_256B(ARMTCNN_ONET_WIDTH) * 2);
#ifdef AR_SCALER_SINGLE_MODE
        AR_HAL_SCALER_CROP_S stCrop;
        stCrop.u32X = int(it->fX0);
        stCrop.u32Y = int(it->fY0);
        stCrop.u32W = int(it->fX1 - it->fX0 + 1);
        stCrop.u32H = int(it->fY1 - it->fY0 + 1);
        s32Ret = CropAndResize(pstImg, &stCrop, &astDstImg[i], 1);
        if(s32Ret)
        {
            printf("Crop/resize error.\n");
            break;
        }
#else
        astCrop[i].u32X = int(it->fX0);
        astCrop[i].u32Y = int(it->fY0);
        astCrop[i].u32W = int(it->fX1 - it->fX0 + 1);
        astCrop[i].u32H = int(it->fY1 - it->fY0 + 1);
#endif
        pRszBuffVirt += ARMTCNN_ONET_HEIGHT * ALIGNED_256B(ARMTCNN_ONET_WIDTH) * 3;
        pRszBuffPhy += ARMTCNN_ONET_HEIGHT * ALIGNED_256B(ARMTCNN_ONET_WIDTH) * 3;
        i++;
    }

#ifndef AR_SCALER_SINGLE_MODE
    AR_U32 u32CropLoop, u32CropLeft;
    u32CropLoop = i / AR_SCALER_MAX_BATCH;
    u32CropLeft = i % AR_SCALER_MAX_BATCH;
    for(i = 0; i < u32CropLoop; i++)
    {
        CropAndResize(pstImg, astCrop, astDstImg, AR_SCALER_MAX_BATCH);
    }
    if(u32CropLeft)
    {
        CropAndResize(pstImg, astCrop + u32CropLoop * AR_SCALER_MAX_BATCH, astDstImg + u32CropLoop * AR_SCALER_MAX_BATCH, u32CropLeft);
    }
#endif

    //run batch Onet
    u32CropCnt = i;
    u32Batch = mOnet.GetBatchNum();
    u32LoopCnt = u32CropCnt / u32Batch;
    u32Left = u32CropCnt % u32Batch;
    it = listTotalBondingBox.begin();

    for(i = 0; i < u32LoopCnt; i++)
    {
    	memset(pstImgSet, 0, sizeof(AR_IMG_SET_S));
		for(AR_U32 j = 0; j < u32Batch; j++)
    	{
			memcpy(&pstImgSet->astInputImg[0].astBatchImg[j],&astDstImg[i*u32Batch+j],sizeof(AR_IMG_S));
			if(mOnet.GetIfcPreProcess()){
				pstImgSet->astInputImg[0].bPreIfcProcess =AR_TRUE;
			}
		}

        mOnet.Forward(pstImgSet, AR_TRUE, AR_FALSE);
        mOnet.PostProcess(ARMTCNN_POST_MODE_ONET);

        u32BoxCnt = mOnet.GetBondingBoxNum();
        if(u32BoxCnt != u32Batch)
        {
            printf("Onet: Mismatch batch and box cnt: %d %d.\n", u32BoxCnt, u32Batch);
            break;
        }
        for(AR_U32 j = 0; j < u32BoxCnt; j++)
        {
            BONDING_BOX_S stBox = mOnet.GetBondingBox();
            //drop the invalid box
            if(!stBox.u32Valid)
            {
                it->u32Valid = 0;
            }
            else
            {
                it->dScore = stBox.dScore;
                it->fDx0 = stBox.fDx0;
                it->fDy0 = stBox.fDy0;
                it->fDx1 = stBox.fDx1;
                it->fDy1 = stBox.fDy1;
            }

            it++;
        }
    }
    if(u32Left)
    {
	    memset(pstImgSet, 0, sizeof(AR_IMG_SET_S));
		for(AR_U32 j = 0; j < u32Left; j++)
    	{
			memcpy(&pstImgSet->astInputImg[0].astBatchImg[j],&astDstImg[u32LoopCnt*u32Batch+j],sizeof(AR_IMG_S));
			if(mOnet.GetIfcPreProcess()){
				pstImgSet->astInputImg[0].bPreIfcProcess =AR_TRUE;
			}
		}

        mRnet.Forward(pstImgSet, AR_TRUE, AR_FALSE);
        mOnet.PostProcess(ARMTCNN_POST_MODE_ONET);

        for(AR_U32 j = 0; j < u32Left; j++)
        {
            BONDING_BOX_S stBox = mOnet.GetBondingBox();
            //drop the invalid box
            if(!stBox.u32Valid)
            {
                it->u32Valid = 0;
            }
            else
            {
                it->dScore = stBox.dScore;
                it->fDx0 = stBox.fDx0;
                it->fDy0 = stBox.fDy0;
                it->fDx1 = stBox.fDx1;
                it->fDy1 = stBox.fDy1;
            }

            it++;
        }
    }

    //delete invalid box, shrink the list
    it = listTotalBondingBox.begin();
    while(it != listTotalBondingBox.end())
    {
        if(!it->u32Valid)
        {
            it = listTotalBondingBox.erase(it);
        }
        else
        {
            dbgd <<"w,h: " << it->u32IndexW -1 << " " << it->u32IndexH -1 << "Score " << it->dScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<"]"
             << " [ " << it->fDx0 << " " << it->fDy0 << " " << it->fDx1 << " " << it->fDy1 <<"]" << endl;

            it++;
        }
    }
    printf("Onet has %ld bbox.\n", listTotalBondingBox.size());
    for(it = listTotalBondingBox.begin(); it != listTotalBondingBox.end(); it++)
    {
        if(it->u32Valid)
        {
            BBoxRegression(it);

            dbgob <<"w,h: " << it->u32IndexW -1 << " " << it->u32IndexH -1 << "Score " << it->dScore << " [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 <<"]"
             << " [ " << it->fDx0 << " " << it->fDy0 << " " << it->fDx1 << " " << it->fDy1 <<"]" << endl;

            it->fX0 = int(CAP(it->fX0, 0, pstImg->u32Width - 1));
            it->fY0 = int(CAP(it->fY0, 0, pstImg->u32Height - 1));
            it->fX1 = int(CAP(it->fX1, 0, pstImg->u32Width - 1));
            it->fY1 = int(CAP(it->fY1, 0, pstImg->u32Height - 1));
        }
    }

    //AR_RELEASE_ION_MEMORY(pDstAddrVirt);
    ReleaseMMZMemory((AR_U64)pDstAddrPhy, (void *)pDstAddrVirt);
	free(pstImgSet);
    free(astDstImg);
    free(astCrop);

    return 0;
}

//#define USE_OPENCV_RESIZE
AR_S32 CARFaceDetector::CropAndResize(AR_IMG_S * pstSrcImg, AR_HAL_SCALER_CROP_S * pstCrop, AR_IMG_S * pstDstImg, AR_U32 u32ImgNum)
{
    #ifndef USE_OPENCV_RESIZE
    #if 0
        static int cnt = 0;
        char tmp[128] = {0};
        cv::Mat img;
        img.create(pstSrcImg->u32Height, pstSrcImg->u32Width, CV_8UC3);
        cv::Mat srcCh[3];
        srcCh[0].create(pstSrcImg->u32Height, pstSrcImg->u32Width, CV_8UC1);
        srcCh[1].create(pstSrcImg->u32Height, pstSrcImg->u32Width, CV_8UC1);
        srcCh[2].create(pstSrcImg->u32Height, pstSrcImg->u32Width, CV_8UC1);
        //R
        memcpy(srcCh[0].data,
            (AR_CHAR *)(pstSrcImg->stRGB.u32RAddrVirt),
            pstSrcImg->u32Width * pstSrcImg->u32Height);
        //G
        memcpy(srcCh[1].data,
            (AR_CHAR *)(pstSrcImg->stRGB.u32RAddrVirt + pstSrcImg->u32Height * pstSrcImg->u32Width),
            pstSrcImg->u32Width * pstSrcImg->u32Height);
        //R
        memcpy(srcCh[0].data,
            (AR_CHAR *)(pstSrcImg->stRGB.u32RAddrVirt + pstSrcImg->u32Height * pstSrcImg->u32Width * 2),
            pstSrcImg->u32Width * pstSrcImg->u32Height);

        cv::merge(srcCh, 3, img);

        sprintf(tmp, "/mnt/src%d.jpg", cnt);
        cv::imwrite(tmp, img);
        cnt++;
      #endif
        return AR_MPI_SCALER_CropResize(pstSrcImg, pstCrop, pstDstImg, u32ImgNum, SCALER_MODE_BILINEAR);
    #else
        std::cout << "start opencv rsz to " << pstDstImg->u32Width << " x " << pstDstImg->u32Height << endl;

        static int cnt = 0;
        char tmp[128] = {0};
        AR_U32 w, h, s;
        cv::Mat img;
        img.create(pstSrcImg->u32Height, pstSrcImg->u32Width, CV_8UC3);
        cv::Mat srcCh[3];
        srcCh[0].create(pstSrcImg->u32Height, pstSrcImg->u32Width, CV_8UC1);
        srcCh[1].create(pstSrcImg->u32Height, pstSrcImg->u32Width, CV_8UC1);
        srcCh[2].create(pstSrcImg->u32Height, pstSrcImg->u32Width, CV_8UC1);
        //B
        memcpy(srcCh[0].data,
            (AR_CHAR *)(pstSrcImg->astChannels[2].uptrAddrVirt),
            pstSrcImg->u32Width * pstSrcImg->u32Height);

        //G
        memcpy(srcCh[1].data,
            (AR_CHAR *)(pstSrcImg->astChannels[1].uptrAddrVirt),
            pstSrcImg->u32Width * pstSrcImg->u32Height);
        //R
        memcpy(srcCh[2].data,
            (AR_CHAR *)(pstSrcImg->astChannels[0].uptrAddrVirt),
            pstSrcImg->u32Width * pstSrcImg->u32Height);

        cv::merge(srcCh, 3, img);

        //sprintf(tmp, "/mnt/src%d.jpg", cnt);
        //cv::imwrite(tmp, img);
        for(int i = 0; i < u32ImgNum; i++)
        {
            cv::Rect roi(pstCrop[i].u32X, pstCrop[i].u32Y, pstCrop[i].u32W, pstCrop[i].u32H);
            cv::Mat crop = img(roi);
            //sprintf(tmp, "/mnt/crop%d.jpg", cnt);
            //cv::imwrite(tmp, crop);
            cv::Mat rsz;
            cv::resize(crop, rsz, cv::Size(pstDstImg[i].u32Width, pstDstImg[i].u32Height));
            //sprintf(tmp, "/mnt/rsz%d_%dx%dcv.bmp", cnt, pstDstImg[i].u32Width, pstDstImg[i].u32Height);
            //cv::imwrite(tmp, rsz);
            cnt++;
            cv::Mat channels[3];
            cv::split(rsz, channels);

            for(h = 0; h < pstDstImg[i].u32Height; h++)
            {
                //R
                memcpy((AR_CHAR *)(pstDstImg[i].astChannels[0].uptrAddrVirt + h * pstDstImg[i].astChannels[0].u32Stride),
                    channels[2].data + h * pstDstImg[i].u32Width,
                    pstDstImg[i].u32Width);
                //G
                memcpy((AR_CHAR *)(pstDstImg[i].astChannels[1].uptrAddrVirt + h * pstDstImg[i].astChannels[1].u32Stride),
                    channels[1].data + h * pstDstImg[i].u32Width,
                    pstDstImg[i].u32Width);
                //B
                memcpy((AR_CHAR *)(pstDstImg[i].astChannels[2].uptrAddrVirt + h * pstDstImg[i].astChannels[2].u32Stride),
                    channels[0].data + h * pstDstImg[i].u32Width,
                    pstDstImg[i].u32Width);
            }
            crop.release();
            rsz.release();
            channels[0].release();
            channels[1].release();
            channels[2].release();
        }

        img.release();

        std::cout << "done opencv rsz " << cnt <<" to " << pstDstImg->u32Width << " x " << pstDstImg->u32Height << endl;
        return 0;
    #endif
}

AR_S32 CARFaceDetector::SetLogLevel(AR_U32 u32LogLevel)
{
    for(AR_U32 i = 0; i < u32PnetNum; i++)
    {
        mPnet[i].SetLogLevel(u32LogLevel);
    }

    mRnet.SetLogLevel(u32LogLevel);
    mOnet.SetLogLevel(u32LogLevel);

    return 0;
}

AR_S32 CARFaceDetector::EnableProfile()
{
    for(AR_U32 i = 0; i < u32PnetNum; i++)
    {
        mPnet[i].EnableProfile();
    }

    mRnet.EnableProfile();
    mOnet.EnableProfile();

    return 0;
}

AR_S32 CARFaceDetector::DisableProfile()
{
    for(AR_U32 i = 0; i < u32PnetNum; i++)
    {
        mPnet[i].DisableProfile();
    }

    mRnet.DisableProfile();
    mOnet.DisableProfile();

    return 0;
}

AR_S32 CARFaceDetector::EnableNPUDebug(AR_U32 u32Start, AR_U32 u32End)
{
    for(AR_U32 i = 0; i < u32PnetNum; i++)
    {
    	mPnet[i].RegisterLayerDebugFunc(DumpLayerData);
        mPnet[i].EnableNPUDebug(u32Start, u32End);
    }

	mRnet.RegisterLayerDebugFunc(DumpLayerData);
	mOnet.RegisterLayerDebugFunc(DumpLayerData);

    mRnet.EnableNPUDebug(u32Start, u32End);
    mOnet.EnableNPUDebug(u32Start, u32End);

    return 0;
}

AR_S32 CARFaceDetector::DisableNPUDebug()
{
    for(AR_U32 i = 0; i < u32PnetNum; i++)
    {
        mPnet[i].DisableNPUDebug();
    }

    mRnet.DisableNPUDebug();
    mOnet.DisableNPUDebug();

    return 0;
}
AR_S32 CARFaceDetector::EnableIFCDebug()
{
    for(AR_U32 i = 0; i < u32PnetNum; i++)
    {
        mPnet[i].RegisterIFCDebugFunc(DumpIFCOutput);
    }

    mRnet.RegisterIFCDebugFunc(DumpIFCOutput);
    mOnet.RegisterIFCDebugFunc(DumpIFCOutput);

    return 0;

}

AR_S32 CARFaceDetector::DrawOsd()
{
    return 0;
}

AR_S32 CARFaceDetector::DrawOsd(AR_CHAR * pFile)
{
    BondingBoxList::iterator it;
    cv::Mat img = cv::imread(pFile);

    for(it = listTotalBondingBox.begin(); it != listTotalBondingBox.end(); it++)
    {
        if(it->u32Valid)
        {
            //cv::Rect r = cv::Rect(it->X0, it->Y0, (it->X1 - it->X0), (it->Y1 - it->X1));
            cv::rectangle(img, cv::Rect(cv::Point(int(it->fX0), int(it->fY0)), cv::Point(int(it->fX1), int(it->fY1)) ), cv::Scalar(255, 0, 0), 2, 8, 0);
        }
    }

    cv::imwrite("/mnt/armtcnn_result.jpg", img);

    return 0;
}

AR_U32 CARFaceDetector::IsInitialized()
{
    return u32Initialized;
}

CARFaceDetector::~CARFaceDetector()
{
	AR_S32 s32Ret = 0;
    listTotalBondingBox.clear();
    u32Initialized = 0;
    s32Ret = AR_MPI_NPU_FreeBuff(&stRuntimeSharedBuff);
    if(s32Ret)
    {
         std::cout << "RuntimeSharedBuff Free ERROR"<< endl;
    }
}

