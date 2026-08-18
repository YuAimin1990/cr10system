/*
 * ar_arm_cb_1.c
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#include "ArArmRmtCallDeclare.h"


extern AR_NETPARAM_LIST_st xGNetParamLists;

AR_S32 ArArmUnRegisterNPUCallback(void *pParams)
{

   AR_U32* xNPUAlgTaskBody = (AR_U32*)pParams;
   AR_U32 u32NetworkId   = *(AR_U32*)xNPUAlgTaskBody;
   AR_U32 au32InputAddr  = *((AR_U32*)xNPUAlgTaskBody +1);
   AR_U32 au32OutputAddr = *((AR_U32*)xNPUAlgTaskBody +2);

   if(u32NetworkId ==0)
   {
		xDebugPrint("NetworkId is ZERO !!!\r\n");
		return -1;
   }
   vArNetParamListRemoveByNetParams(&(xGNetParamLists.list),u32NetworkId,au32InputAddr,au32OutputAddr);

   xDebugPrint("ArArmUnRegisterNPUCallback Done\r\n");
   return 0;

}


AR_ARM_REMOTECALL_DECLARE(ArArmUnRegisterNPUCallback) = {
    .pName = "ArArmUnRegisterNPUCallback",
    .pRmtCallHandler = ArArmUnRegisterNPUCallback,
};

