/*
 * system_IO.h
 *
 *  Created on: 2017Äê4ÔÂ5ÈÕ
 *      Author: jianliu
 */

#ifndef SYSTEMIO_H_
#define SYSTEMIO_H_
#include "IoOperation.h"
#include "RegisterAddr.h"
#ifdef __cplusplus
extern "C"{
#endif


#define vConfigAddxAttribute() cpm_out(ADD0_ATT1, 0x44)
#define xGetSelfCoreId() cpm_in(CORE_ID_CPM_ADDR)

#define vEnableAllMcciInterrupt() vInternalCpmWrite(COMM_INT_EN_ADDR,0Xffffffff)
#define vConfigMcciInterrupt(core_id) vIoBitSet(CORE0_INT0_MASK_OFFSET + CORE_ADDR_INC_INT * core_id, MCCI_MES_INTERRUPT_BIT)
#define vClearMcciStsWholeRegSelf() vInternalCpmWrite(MCCI_STS_OFFSET, 0xffffffff)

#define vMaskDbgGen(mask) vInternalCpmWrite(DBG_GEN_MASK, mask)
#define vMaskDbgGen2(mask) vInternalCpmWrite(DBG_GEN_2_MASK, mask)

#define vTriggerToArmInterrupt0(core_id) vIoWrite(core_id * TO_ARM_INTERRUPT_CORE_OFFSET + TO_ARM_INTERRUPT0_BASE, 1)
#define vClearToArmInterrupt0(core_id) vIoWrite(core_id * TO_ARM_INTERRUPT_CORE_OFFSET + TO_ARM_INTERRUPT0_BASE, 0)
#define xReadToArmInterrupt0(core_id) xIoRead(core_id * TO_ARM_INTERRUPT_CORE_OFFSET + TO_ARM_INTERRUPT0_BASE)
#define vEnableToArmInterrupt0(core_id) vIoWrite(core_id * TO_ARM_INTERRUPT_CORE_OFFSET + TO_ARM_INTERRUPT0_MASK_BASE, 1)
#define xReadToArmInterrupt0_mask(core_id) xIoRead(core_id * TO_ARM_INTERRUPT_CORE_OFFSET + TO_ARM_INTERRUPT0_MASK_BASE)

#define vTriggerToArmInterrupt1(core_id, value) vIoWrite(core_id * TO_ARM_INTERRUPT_CORE_OFFSET + TO_ARM_INTERRUPT1_BASE, (value))
#define vClearToArmInterrupt1(core_id) vIoWrite(core_id * TO_ARM_INTERRUPT_CORE_OFFSET + TO_ARM_INTERRUPT1_BASE, 0)
#define xReadToArmInterrupt1(core_id) xIoRead(core_id * TO_ARM_INTERRUPT_CORE_OFFSET + TO_ARM_INTERRUPT1_BASE)
#define vEnableToArmInterrupt1(core_id) vIoWrite(core_id * TO_ARM_INTERRUPT_CORE_OFFSET + TO_ARM_INTERRUPT1_MASK_BASE, 0xffffffff)
#define xReadToArmInterrupt1Mask(core_id) xIoRead(core_id * TO_ARM_INTERRUPT_CORE_OFFSET + TO_ARM_INTERRUPT1_MASK_BASE)



#ifdef __cplusplus
}
#endif
#endif /* SYSTEMIO_H_ */
