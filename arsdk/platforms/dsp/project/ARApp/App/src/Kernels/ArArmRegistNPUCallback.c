/*
 * ar_arm_cb_1.c
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#include "ArArmRmtCallDeclare.h"


extern AR_NETPARAM_LIST_st xGNetParamLists;

AR_S32 ArArmRegisterNPUCallback(void *pParams)
{
   AR_S32 ret = 0;
   AR_U64 u64TimeStart = 0, u64TimeEnd = 0;

   AR_U32* xNPUAlgTaskBody = (AR_U32*)pParams;
   if(xArNetParamListIsExistByNetWorkId(&(xGNetParamLists.list),*(AR_U32*)xNPUAlgTaskBody))
   {
	 	xDebugPrint("NetworkId Node Has Already Existed, Just Return Directly!\r\n");
		return -1;
   }

   AR_NPU_LIST_ITEMS_st *NpuListItem = NULL;
   NpuListItem = (AR_NPU_LIST_ITEMS_st *)malloc(sizeof(AR_NPU_LIST_ITEMS_st));
   if(NpuListItem == NULL)
   {
	   xDebugPrint("NpuListItem malloc failed\r\n");
	   return -1;
   }
   memset(NpuListItem, 0 , sizeof(AR_NPU_LIST_ITEMS_st));

   NpuListItem->u32NetworkId   = *(AR_U32*)xNPUAlgTaskBody;
   //NpuListItem->au32InputAddr  = *((AR_U32*)xNPUAlgTaskBody +1);
   //NpuListItem->au32OutputAddr = *((AR_U32*)xNPUAlgTaskBody +2);
   if(NpuListItem->u32NetworkId ==0)
   {
	   xDebugPrint("NetworkId   is ZERO !!!\r\n");
	   free(NpuListItem);
	   return -1;
   }

   AR_U32 pu8FilePhyAddr = *((AR_U32*)xNPUAlgTaskBody +1);
   AR_CHAR* pcJson=(AR_CHAR*)pu8FilePhyAddr;
   if(pcJson == NULL)
   {
		 xDebugPrint("cJson file is Null\r\n");
		 free(NpuListItem);
		 return -1;
   }

   AR_U32 u32CBNum = 0;
   AR_NPU_CB_PARAM_S * pstCB = NpuListItem->astCBParam;
   u64TimeStart = mss_get_clock();
   ret = xArNpuParseCBFile(pcJson, pstCB, &u32CBNum);
   u64TimeEnd = mss_get_clock();
   xDebugPrint("Parse CB file runtime cycyle: %lld\r\n", u64TimeEnd - u64TimeStart);
   if(ret < 0)
   {
	   xDebugPrint("Parse CB failed\r\n");
	   free(NpuListItem);
	   return -1;
   }

   xArNetParamListAdd(&(xGNetParamLists.list),NpuListItem);
   xDebugPrint("ArArmRegisterNPUCallback Done\r\n");

   return 0;

}


AR_ARM_REMOTECALL_DECLARE(ArArmRegisterNPUCallback) = {
    .pName = "ArArmRegisterNPUCallback",
    .pRmtCallHandler = ArArmRegisterNPUCallback,
};

