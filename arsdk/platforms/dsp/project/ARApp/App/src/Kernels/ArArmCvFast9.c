/*
 * ArArmCv_test.c
 *
 *  Created on: 2022Äê6ÔÂ1ÈÕ
 *      Author: syli
 */



#include "Uart.h"
#include "mss.h"
#include "ArArmRmtCallDeclare.h"
#include "fast9_wrapper.h"



AR_S32 ArArmCvFast9(void* xParams)
{
    AR_U32* xNPUAlgTaskBody =  (AR_U32*)xParams;
    AR_U32 in_addr          = *((AR_U32*)xNPUAlgTaskBody +0);// yuv input
    AR_U32 out_x         	= *((AR_U32*)xNPUAlgTaskBody +1);
    AR_U32 out_y         	= *((AR_U32*)xNPUAlgTaskBody +2);
    AR_U32 out_s         	= *((AR_U32*)xNPUAlgTaskBody +3);
    AR_U32 width          	= *((AR_U32*)xNPUAlgTaskBody +4);
    AR_U32 height         	= *((AR_U32*)xNPUAlgTaskBody +5);
    AR_U32 threshold        = *((AR_U32*)xNPUAlgTaskBody +6);
    AR_U32 nms         		= *((AR_U32*)xNPUAlgTaskBody +7);
    AR_U32 output_list_is_external  = *((AR_U32*)xNPUAlgTaskBody +8);
    AR_U32 use_grid 		= *((AR_U32*)xNPUAlgTaskBody +9);

    AR_U32 num_points 		= 0;

	int ret;
	uchar* input_addr=(uchar*)in_addr;
	ushort* px = (ushort*)out_x;
	ushort* py = (ushort*)out_y;
	ushort* ps = (ushort*)out_s;

	xDebugPrint("input_addr = %p\r\n",input_addr);
	xDebugPrint("px = %p\r\n",px);
	xDebugPrint("py = %p\r\n",py);
	xDebugPrint("ps = %p\r\n",ps);

	xDebugPrint("width = %x\r\n",width);
	xDebugPrint("height = %x\r\n",height);
	xDebugPrint("threshold = %x\r\n",threshold);

 	xDebugPrint("nms = %x\r\n",nms);
 	xDebugPrint("output_list_is_external = %x\r\n",output_list_is_external);
 	xDebugPrint("use_grid = %x\r\n",use_grid);

    unsigned long long start, end;

    start = mss_get_clock();
  	ret = cvFast9_process(input_addr,width, height,px,py,ps, threshold,  (uchar)nms, (uchar)output_list_is_external, (uchar)use_grid,(uint*)&num_points);
  	if(ret <0)
  	      	xDebugPrint("cvFast9_process failed!\r\n");

  	end = mss_get_clock();
  	xDebugPrint("cvFast9 run time:%llu .\n",end-start);

  	xDebugPrint("num_points = %d\r\n",num_points);
	for(short i=0;i<num_points;i++){
		xDebugPrint("{%d},x=%d,y=%d,score=%d \r\n",i,px[i],py[i],ps[i]);
	 }


	return 0;

}



AR_ARM_REMOTECALL_DECLARE(ArArmCvFast9) = {
    .pName = "ArArmCvFast9",
    .pRmtCallHandler = ArArmCvFast9,
};





