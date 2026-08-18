
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

#include "canny_wrapper.h"




/*
    * \param[in]  in_addr   			Input frame
    * \param[out] out_addr  			Output frame
    * \param[in]  src_width    			width of source frame
    * \param[in]  src_height  			height of source frame
    * \param[in]  src_stride    		stride of source frame
    * \param[in]  bit_depth  			8,16
    * \param[in]  high_threshold    		
    * \param[in]  low_threshold  		
    *
 * */



AR_S32 ArArmCvCanny(void* xParams)
{
    AR_U32* xNPUAlgTaskBody =  (AR_U32*)xParams;

    AR_U32 in_addr          = *((AR_U32*)xNPUAlgTaskBody +0);
    AR_U32 out_addr         = *((AR_U32*)xNPUAlgTaskBody +1);

    AR_U32 src_width  		= *((AR_U32*)xNPUAlgTaskBody +2);
    AR_U32 src_height 		= *((AR_U32*)xNPUAlgTaskBody +3);
    AR_U32 src_stride  		= *((AR_U32*)xNPUAlgTaskBody +4);

    AR_U32 bit_depth            = *((AR_U32*)xNPUAlgTaskBody +5);
    AR_U32 high_threshold     	= *((AR_U32*)xNPUAlgTaskBody +6);
    AR_U32 low_threshold        = *((AR_U32*)xNPUAlgTaskBody +7);


	xDebugPrint("in_addr %x\r\n",in_addr);
	xDebugPrint("out_addr %x\r\n",out_addr);
	xDebugPrint("src_width %d\r\n",src_width);
	xDebugPrint("src_height %d\r\n",src_height);
	xDebugPrint("src_stride %d\r\n",src_stride);
	xDebugPrint("bit_depth %d\r\n",bit_depth);
	xDebugPrint("high_threshold %d\r\n",high_threshold);
	xDebugPrint("low_threshold %d\r\n",low_threshold);


	unsigned char *src_buf = (unsigned char *)in_addr;
	unsigned char * dst_buf = (unsigned char *)out_addr;

	unsigned long long start, end;

	start = mss_get_clock();

	int ret = cvCanny(src_buf, dst_buf,bit_depth,src_width,src_height,src_stride,high_threshold,low_threshold);

	end = mss_get_clock();
	xDebugPrint("canny run time:%llu .\n",end-start);

	return 0;
}



AR_ARM_REMOTECALL_DECLARE(ArArmCvCanny) = {
    .pName = "ArArmCvCanny",
    .pRmtCallHandler = ArArmCvCanny,
};




