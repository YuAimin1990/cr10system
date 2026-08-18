/*
 * register_addr.h
 *
 *  Created on: 2017Äê3ÔÂ21ÈÕ
 *      Author: jianliu
 */
#ifndef REGISTERADDR_H_
#define REGISTERADDR_H_
#ifdef __cplusplus
extern "C"{
#endif

//=============memory attribution============//
#define ADD_ADDR_OFFSET (0x10)
#define ADD0_START (0x920)
#define ADD0_ATT0  (0x924)
#define ADD0_ATT1  (0x928)
#define P_ADD_ADDR_OFFSET (0x10)
#define P_ADD0_START (0x414)
#define P_ADD0_ATT0  (0x418)
#define MSS_PCR (0x404)
#define MSS_SDCFG (0x62c)
//=========core==========================//
#define CORE_ID_CPM_ADDR (0x178)

//=========mcci==========================//
#define COMM_INT_EN_ADDR 0x84
#define MCCI_STS_OFFSET 0X80


#define REGISTER_BASE_ADDR 0x010C0000
#define CORE_ADDR_INC_INT 0X1000
#define MCCI_MES_INTERRUPT_BIT 0

#define CORE0_INT0_MASK_OFFSET (REGISTER_BASE_ADDR + 0x4000)
#define CORE0_INT1_MASK_OFFSET (REGISTER_BASE_ADDR + 0x4004)
#define CORE0_INT2_MASK_OFFSET (REGISTER_BASE_ADDR + 0x400c)


//==========debg gen============================//
#define DBG_GEN_MASK (0Xd28)
#define DBG_GEN_2_MASK (0Xd2c)

#define DBG_STACK_START (0xd44)
#define DBG_STACK_END (0xd48)

//==============int0/1==========================//
#define TO_ARM_INTERRUPT_CORE_OFFSET 0x1000
#define TO_ARM_INTERRUPT0_BASE (REGISTER_BASE_ADDR + 0x4020)
#define TO_ARM_INTERRUPT0_MASK_BASE (REGISTER_BASE_ADDR + 0x4024)
#define TO_ARM_INTERRUPT1_BASE (REGISTER_BASE_ADDR + 0x4028)
#define TO_ARM_INTERRUPT1_MASK_BASE (REGISTER_BASE_ADDR + 0x402C)


#ifdef __cplusplus
}
#endif
#endif /* REGISTER_ADDR_H_ */
