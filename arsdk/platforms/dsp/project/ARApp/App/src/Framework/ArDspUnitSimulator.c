
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Uart.h"
#include "ArMalloc.h"
#include "ArNpuPlatform.h"
#include "ArArmRmtCallDeclare.h"
#include "ArCnnOperatorDeclare.h"
#include "ArDspUnitSimulator.h"


static int xParserCallbackJson(void *pCbJsonAddr, AR_NPU_CB_PARAM_S *CallBackParam, int* pCallbackNum)
{
   int ret = -1;
   int i =0;

   AR_NPU_LIST_ITEMS_st *NpuListItem = (AR_NPU_LIST_ITEMS_st *)malloc(sizeof(AR_NPU_LIST_ITEMS_st));
   if(NpuListItem == NULL)
   {
	   xDebugPrint("NpuListItem malloc failed\r\n");
	   return ret;
   }

   AR_NPU_CB_PARAM_S * pstCB = NpuListItem->astCBParam;
   ret = xArNpuParseCBFile((AR_CHAR*)pCbJsonAddr, pstCB, pCallbackNum);
   if(ret < 0)
   {
	   xDebugPrint("Parse CB failed\r\n");
	   free(NpuListItem);
	   return ret;
   }

   for( i =0;i< *pCallbackNum; i++)
   {
	  memcpy((void*)&CallBackParam[i],(void*)&NpuListItem->astCBParam[i],sizeof(AR_NPU_CB_PARAM_S));
	  xDebugPrint("CB[%d] opName %s opType %s\r\n",i,CallBackParam[i].achOperatorName,CallBackParam[i].achOperatorType);
   }

   free(NpuListItem);

   return ret;
}



int xLocalFileRead(char *vFilePath, char **pBuff)
 {
	 xDebugPrint("filename %s  \r\n",vFilePath);
	 FILE *fp;
	 char *FileBuff = NULL;
	 int ReadfileSize = 0;
	 int RealFileSize = 0;

	 fp = fopen(vFilePath, "rb");
	 if(!fp)
	 {
		 xDebugPrint("fopen %s failed! \r\n",vFilePath);
		 return -1;
	 }

	 fseek(fp, 0L, SEEK_END);
	 RealFileSize = ftell(fp);
	 rewind(fp);

	 FileBuff = malloc(RealFileSize +4);
	 memset(FileBuff,0,RealFileSize +4);


	 ReadfileSize = fread(FileBuff, 1, RealFileSize, fp);
	 xDebugPrint("ReadfileSize=%d,RealFileSize=%d \r\n",ReadfileSize,RealFileSize);

	 fclose(fp);
	 if(ReadfileSize != RealFileSize)
	 {
		 xDebugPrint("readfile_size != RealFileSize, return error \r\n");
		 return -1;
	 }

	 *pBuff = FileBuff;
	 return ReadfileSize;
 }

int xLocalFileWrite(char *vFilePath, char *pBuff, unsigned int BufferSize)
{
	 FILE *fp;
	 //int RealFileSize = 0;
	 int WriteFileSize = 0;

	 fp = fopen(vFilePath, "w+");
	 if(!fp)
	 {
		 return -1;
	 }

	 //fseek(fp, 0L, SEEK_END);
	 //RealFileSize = ftell(fp);
	 //rewind(fp);
	 //fseek(fp, 0, SEEK_SET);
	 WriteFileSize = fwrite(pBuff, 1, BufferSize, fp);

	 fclose(fp);

	 xDebugPrint("write WriteFileSize = %d BufferSize =%d \r\n",WriteFileSize,BufferSize);

	 if(WriteFileSize != BufferSize)
	 {
		 xDebugPrint("WriteFileSize 1=BufferSize , return error\r\n");
		 return -1;
	 }

	 return 0;
}


 int xArBareMetalUnitSimulatorWithNpu(void *vDspInputFile, void *vDspOutputFile, void *CjsonFile, void *vDspGoldenFile)
 {
     int ret = 0;
     int i = 0;

     if((!vDspInputFile) || (!vDspGoldenFile) || (!CjsonFile))
     {
     	 xDebugPrint("in/golden/json file is null!\r\n");
    	 return -1;
     }

	 char *CjsonfileBuff = NULL;
	 int CjsonfileSize = 0;
     CjsonfileSize = xLocalFileRead((char *)CjsonFile, &CjsonfileBuff);
     if(CjsonfileSize < 0)
     {
    	 xDebugPrint("load json file failed!\r\n");
    	 if(CjsonfileBuff)
    		 free(CjsonfileBuff);
    	 return -1;
     }
     xDebugPrint("load json file success!\r\n");

	 char *InputfileBuff = NULL;
	 int InputfileSize = xLocalFileRead((char *)vDspInputFile, &InputfileBuff);
	 if(InputfileSize < 0)
	 {
		 xDebugPrint("read vDspInputFile failed!\r\n");
		 if(CjsonfileBuff)
		     free(CjsonfileBuff);
		 if(InputfileBuff)
			 free(InputfileBuff);
		 return -1;
	 }
	 xDebugPrint("load input file success!\r\n");

	 char * GoldenfileBuff = NULL;
	 int GoldenSize = xLocalFileRead((char *)vDspGoldenFile, &GoldenfileBuff);
	 if(GoldenSize < 0)
	 {
		 xDebugPrint("read golden failed!\r\n");
		 if(CjsonfileBuff)
			 free(CjsonfileBuff);
		 if(InputfileBuff)
			 free(InputfileBuff);
		 if(GoldenfileBuff)
			 free(GoldenfileBuff);
		 return -1;
	 }
	 xDebugPrint("load golden file success!\r\n");

	 char *OutputfileBuff = NULL;
	 OutputfileBuff = (char *)malloc(GoldenSize);
	 if(!OutputfileBuff)
	 {
		 xDebugPrint("malloc OutputfileBuff failed !\r\n");
		 if(CjsonfileBuff)
			 free(CjsonfileBuff);
		 if(InputfileBuff)
			 free(InputfileBuff);
		 if(GoldenfileBuff)
			 free(GoldenfileBuff);
		 return -1;
	 }
	 memcpy(OutputfileBuff, GoldenfileBuff, GoldenSize);


     int CallbackNum=0;
     AR_NPU_CB_PARAM_S * CallBackParam = (AR_NPU_CB_PARAM_S *)malloc(10 * sizeof(AR_NPU_CB_PARAM_S));
     memset(CallBackParam, 0, 10 * sizeof(AR_NPU_CB_PARAM_S));
	 //xDebugPrint("opName %d opType %s\r\n",CallBackParam->u32Id, CallBackParam->achOperatorName);
     ret = xParserCallbackJson((void *)CjsonfileBuff, CallBackParam, &CallbackNum);
     if(ret < 0)
     {
    	 xDebugPrint("xParserCallbackJson failed!\r\n");
    	 goto exit_handler;
     }
	 xDebugPrint("CallbackNum = %d \r\n",CallbackNum);

	 for(i=0;i<CallbackNum;i++)
	 {
		 unsigned int CbInOffset = CallBackParam[i].astInputTensor[0].u32Offset;
		 unsigned int CbOutOffset = CallBackParam[i].astOutputTensor[0].u32Offset;
		 xDebugPrint("CB[%d] InOffset=%x OutOffset=%x\r\n",i,CbInOffset,CbOutOffset);

		 ArCnnOperatorDeclare_t * entry=NULL;
		 entry = xArNpuOperatorLookUpName(&CallBackParam[i]);
		 if(entry)
		 {
			 ret=entry->pCnnCbHandler((AR_U32)InputfileBuff, (AR_U32)OutputfileBuff,&CallBackParam[i], (void*)CallBackParam[i].pOpParams);
			 if(ret < 0)
			 {
				 goto exit_handler;
			 }
		 }
		 xDebugPrint("CB[%d] Handle Succeed !\r\n",i);
	 }

	 ret = memcmp(OutputfileBuff, GoldenfileBuff, GoldenSize);
	 if(ret)
	 {
		 xDebugPrint("Golden cmp with Output is not same \r\n");
	 }

	 ret = xLocalFileWrite((char *)vDspOutputFile,(char *)OutputfileBuff, GoldenSize);

	 for(i=0;i<CallbackNum;i++)
     {
         if(CallBackParam[i].pOpParams)
         {
			free(CallBackParam[i].pOpParams);
		 }
	 }

exit_handler:
	 if(CallBackParam)
		 free(CallBackParam);
	 if(OutputfileBuff)
		 free(OutputfileBuff);
	 if(InputfileBuff)
		 free(InputfileBuff);
	 if(GoldenfileBuff)
		 free(GoldenfileBuff);
	 if(CjsonfileBuff)
		 free(CjsonfileBuff);

	 return ret;
 }

int xArUnitSimulatorLoadModel(Sim_Model_Handle_S *SimHandle,char *InputFile, char *GoldenFile)
{
	memset(SimHandle, 0 , sizeof(Sim_Model_Handle_S));

	int InputfileSize = 0;
	char *InputfileBuff = NULL;
	InputfileSize = xLocalFileRead((char *)InputFile, &InputfileBuff);
	if(InputfileSize < 0)
	{
		xDebugPrint("read InputFile failed!\r\n");
		if(InputfileBuff)
			free(InputfileBuff);
		return -1;
	}

	int GoldenfileSize = 0;
	char *GoldenfileBuff = NULL;
	GoldenfileSize = xLocalFileRead((char *)GoldenFile, &GoldenfileBuff);
	if(GoldenfileSize < 0)
	{
		xDebugPrint("read GoldenFile failed!\r\n");
		if(InputfileBuff)
			free(InputfileBuff);
		if(GoldenfileBuff)
			free(GoldenfileBuff);
		return -1;
	}

	int OutputfileSize = GoldenfileSize;
	char *OutputfileBuff = NULL;
	OutputfileBuff = (char *)malloc(OutputfileSize);
	if(!OutputfileBuff)
	{
		xDebugPrint("malloc output addr failed!\r\n");
		if(InputfileBuff)
			free(InputfileBuff);
		if(GoldenfileBuff)
			free(GoldenfileBuff);
		return -1;
	}
	memset(OutputfileBuff, 0, OutputfileSize);

	SimHandle->u32InputAddress = (AR_U32)InputfileBuff;
	SimHandle->u32InputSize = InputfileSize;
	SimHandle->u32OutputAddress = (AR_U32)OutputfileBuff;
	SimHandle->u32OutputSize = OutputfileSize;
	SimHandle->u32GoldenAddress = (AR_U32)GoldenfileBuff;
	SimHandle->u32GoldenSize = GoldenfileSize;
	xDebugPrint("InputfileBuff = 0x%x, InputfileSize = %d\n",SimHandle->u32InputAddress, SimHandle->u32InputSize);
	xDebugPrint("OutputfileBuff = 0x%x, OutputfileSize = %d\n",SimHandle->u32OutputAddress, SimHandle->u32OutputSize);
	xDebugPrint("u32GoldenAddress = 0x%x, GoldenfileSize = %d\n",SimHandle->u32GoldenAddress, SimHandle->u32GoldenSize);

	return 0;
}

int xArUnitSimulatorUnLoadModel(Sim_Model_Handle_S *SimHandle,char *OutputFile)
{
	int ret = 0;
	ret = memcmp((char *)SimHandle->u32GoldenAddress, (char *)SimHandle->u32OutputAddress, SimHandle->u32OutputSize);
	if(ret)
	{
		xDebugPrint(" Golden file vs Output File not same \r\n");
	}

	ret = xLocalFileWrite((char *)OutputFile, (char *)SimHandle->u32OutputAddress, SimHandle->u32OutputSize);
	if(ret)
	{
		xDebugPrint(" OutputFile Write error \r\n");
	}

	free((char *)SimHandle->u32InputAddress);
	free((char *)SimHandle->u32OutputAddress);
	free((char *)SimHandle->u32GoldenAddress);

	return 0;
}


int xArBareMetalUnitSimulatorWithArm(void *UserDefinedParam, char *OperatorName)
{
	 int ret = 0;
	 ArARMRemoteCallDeclare_t* entry = NULL;

	 entry = xArArmRemoteCallLookUpName(OperatorName);
	 if(entry)
	 {
		 ret=entry->pRmtCallHandler((void*)UserDefinedParam);
	 }
	 xDebugPrint("%s Process Done RET = %d\r\n",OperatorName, ret);

	 return 0;
}
