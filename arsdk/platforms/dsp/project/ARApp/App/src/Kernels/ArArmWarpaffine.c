
#include "Uart.h"
#include "ArArmRmtCallDeclare.h"


/*
 * ArArmCvApp.c
 *
 *  Created on: 2021骞�9鏈�7鏃�
 *      Author: syli
 */


#include "Uart.h"
#include "mss.h"
#include "ArArmRmtCallDeclare.h"

#include "warp_affine_wrapper.h"




/*
    * \param[in]  in_addr   			Input frame
    * \param[out] out_addr  			Output frame
    * \param[in]  src_width    			width of source frame
    * \param[in]  src_height  			height of source frame
    * \param[in]  dst_width    			width of destinate frame
    * \param[in]  dst_height  			height of destinate frame
    * \param[in]  roi_top_left_x    	top left x	of ROI
    * \param[in]  roi_top_left_y  		top left y	of ROI
    * \param[in]  roi_width    			width of ROI
    * \param[in]  roi_height  			height of ROI
    * \param[in]  fill_val  			fill value:128
    * \param[in]  bit_depth  			8,16
    * \param[in]  ele_size     			Image element size (in bytes)// 1,2
    * \param[in]  M   					rotate matrix
    param.M[0][0] = (double)M[0];
	param.M[0][1] = (double)M[1];
	param.M[0][2] = (double)M[2];
	param.M[1][0] = (double)M[3];
	param.M[1][1] = (double)M[4];
	param.M[1][2] = (double)M[5];
    *
    *
 * */



AR_S32 ArArmCvWarpAffine(void* xParams)
{
    AR_U32* xNPUAlgTaskBody =  (AR_U32*)xParams;

    AR_U32 in_addr          = *((AR_U32*)xNPUAlgTaskBody +0);
    AR_U32 out_addr         = *((AR_U32*)xNPUAlgTaskBody +1);
    AR_U32 src_width  		= *((AR_U32*)xNPUAlgTaskBody +2); //1920
    AR_U32 src_height 		= *((AR_U32*)xNPUAlgTaskBody +3); //1080
    AR_U32 dst_width  		= *((AR_U32*)xNPUAlgTaskBody +4); //1920
    AR_U32 dst_height 		= *((AR_U32*)xNPUAlgTaskBody +5); //1080
    AR_U32 roi_top_left_x   = *((AR_U32*)xNPUAlgTaskBody +6); //20
    AR_U32 roi_top_left_y   = *((AR_U32*)xNPUAlgTaskBody +7); //40
    AR_U32 bit_depth        = *((AR_U32*)xNPUAlgTaskBody +8); //1
    AR_U32 ele_size     	= *((AR_U32*)xNPUAlgTaskBody +9); //1
    AR_U32 roi_width        = *((AR_U32*)xNPUAlgTaskBody +10); //1880
    AR_U32 roi_height       = *((AR_U32*)xNPUAlgTaskBody +11); //1000
    AR_U32 fill_val         = *((AR_U32*)xNPUAlgTaskBody +12); //128
    AR_U32 M 				= *((AR_U32*)xNPUAlgTaskBody +13);

	xDebugPrint("in_addr %x\r\n",in_addr);
	xDebugPrint("out_addr %x\r\n",out_addr);
	xDebugPrint("src_width %d\r\n",src_width);
	xDebugPrint("src_height %d\r\n",src_height);
	xDebugPrint("dst_width %d\r\n",dst_width);
	xDebugPrint("dst_height %d\r\n",dst_height);
	xDebugPrint("roi_top_left_x %d\r\n",roi_top_left_x);
	xDebugPrint("roi_top_left_y %d\r\n",roi_top_left_y);
	xDebugPrint("roi_width %d\r\n",roi_width);
	xDebugPrint("roi_height %d\r\n",roi_height);
	xDebugPrint("fill_val %d\r\n",fill_val);
	xDebugPrint("M[0] %f\r\n",*(float*)(M));
	xDebugPrint("M[1] %f\r\n",*(float*)(M+4));
	xDebugPrint("M[2] %f\r\n",*(float*)(M+8));

	unsigned char * src_buf[1];
	src_buf[0]= (unsigned char *)in_addr;
	unsigned char * dst_buf[1] ;
	dst_buf[0]= (unsigned char *)out_addr;
	unsigned long long start, end;


	start = mss_get_clock();
	int ret = cvWarpAffine(src_buf, dst_buf,bit_depth,ele_size,src_width,src_height,dst_width,dst_height,roi_top_left_x,
			roi_top_left_y,roi_width,roi_height, fill_val,(float*)M);


	end = mss_get_clock();
	xDebugPrint("run time:%llu .\n",end-start);

	return 0;
}





AR_ARM_REMOTECALL_DECLARE(ArArmCvWarpAffine) = {
    .pName = "ArArmCvWarpAffine",
    .pRmtCallHandler = ArArmCvWarpAffine,
};




