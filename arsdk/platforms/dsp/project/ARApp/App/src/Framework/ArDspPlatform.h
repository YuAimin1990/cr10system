/*
*********************************************************************************************
** File:         ar_com.h
**
** Version:      1.0.0.0
**
** Brief:
**
** Copyright(c) 2020 by artosyn. All rights reserved.
** Created on:  August 26th.2020
**
*********************************************************************************************
*/
#ifndef __AR_DSP_PLATFORM_H__
#define __AR_DSP_PLATFORM_H__

#include "cevaxm.h"
//******************DATA TYPE*************************************

#ifdef __CROSS_PLATFORM__
typedef uint32_t     AR_U32;
typedef int32_t      AR_S32;
typedef uint16_t     AR_U16;
typedef int16_t      AR_S16;
typedef uint8_t      AR_UCHAR;
typedef int8_t       AR_CHAR;
typedef uint32_t     AR_BOOL;
typedef double       AR_DOUBLE;
typedef float        AR_FLOAT;
typedef uint64_t     AR_U64;
typedef int64_t      AR_S64;

#else

typedef uint32_t     AR_U32;
typedef int32_t      AR_S32;
typedef uint16_t     AR_U16;
typedef int16_t      AR_S16;
typedef int8_t       AR_S8;
typedef uint8_t      AR_U8;
typedef unsigned char AR_UCHAR;
typedef char         AR_CHAR;
typedef uint32_t     AR_BOOL;
typedef double       AR_DOUBLE;
typedef float        AR_FLOAT;
typedef uint64_t     AR_U64;
typedef int64_t      AR_S64;

#endif

#define AR_FAILURE -1
#define AR_SUCCESS 0

//*******************INTER TIMER*************************************

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */

//#define CEVA_TIMER0_BASE_ADDR  		0x62402000
#define CEVA_TIMER0_BASE_ADDR  		0x1c02000      // update 20210107
#define CEVA_TIMER0_CFG_ADDR  		(CEVA_TIMER0_BASE_ADDR + 0)
#define CEVA_TIMER0_EW_ADDR  		(CEVA_TIMER0_BASE_ADDR + 4)
#define CEVA_TIMER0_CC_ADDR  		(CEVA_TIMER0_BASE_ADDR + 8)
#define CEVA_TIMER0_SC_ADDR  		(CEVA_TIMER0_BASE_ADDR + 0xc)


#define TIMER_CFG_TS_Pos                0UL
#define TIMER_CLOCK_DIVIDED_1          (0UL << TIMER_CFG_TS_Pos)  //reset default
#define TIMER_CLOCK_DIVIDED_2          (1UL << DMAC_CTRL_SDEC_Pos)
#define TIMER_CLOCK_DIVIDED_4          (2UL << DMAC_CTRL_SDEC_Pos)
#define TIMER_CLOCK_DIVIDED_16         (3UL << DMAC_CTRL_SDEC_Pos)


#define TIMER_CFG_CM_Pos               2UL
#define TIMER_SINGLE_COUNT            (0UL << TIMER_CFG_CM_Pos)  //reset default
#define TIMER_AUTO_RESET              (1UL << TIMER_CFG_CM_Pos)
#define TIMER_FREE_RUN   	          (2UL << TIMER_CFG_CM_Pos)
#define TIMER_ENVENT_CONT             (3UL << TIMER_CFG_CM_Pos)


#define TIMER_CFG_ES_Pos              5UL
#define TIMER_INTERNAL_EVENT          (0UL << TIMER_CFG_ES_Pos)  //reset default
#define TIMER_EXTERNALL_EVENT         (1UL << TIMER_CFG_ES_Pos)

#define TIMER_CFG_TREST_EN_Pos        9UL
#define TIMER_DIS			          (0UL << TIMER_CFG_TREST_EN_Pos)  //reset default
#define TIMER_EN         			  (1UL << TIMER_CFG_TREST_EN_Pos)

#define TIMER_CFG_RES_Pos        	  16UL
#define TIMER_RES_0			          (0UL << TIMER_CFG_RES_Pos)     //reset default
#define TIMER_RES_RELOAD   			  (1UL << TIMER_CFG_RES_Pos)

#define TIMER_CFG_CT_Pos              18UL
#define TIMER_CT_0			          (0UL << TIMER_CFG_CT_Pos)  //reset default
#define TIMER_CT_CLR      			  (1UL << TIMER_CFG_CT_Pos)  //

#define TIMER_CFG_RU_Pos              19UL
#define TIMER_RU_0			          (0UL << TIMER_CFG_RU_Pos)  //reset default
#define TIMER_RU_NOT_LATCHED    	  (1UL << TIMER_CFG_RU_Pos)  //


//================APB TIMER======================================================================
#define CEVAXM6_APB_TIMER1_LOAD      0X10C2000
#define CEVAXM6_APB_TIMER1_VALUE     0X10C2004
#define CEVAXM6_APB_TIMER1_CONTR     0X10C2008
#define CEVAXM6_APB_TIMER1_CLEAR     0X10C200C

#define CEVAXM6_APB_INTERRUPT_CLEAR  0X10C20A4

#define CEVAXM6_APB_TIMER_NUM   8
#define CEVAXM6_APB_TIMER_OFFSET    0X14
//----------------------INT-------------------------------------------------
#define CORE0_INT0_MASK_ADDR  		           0X10C4000
#define CORE0_INT1_MASK_ADDR  		           0X10C4004
#define CORE0_INT2_MASK_ADDR  		           0X10C400C
#define CORE0_TO_ARM_UOP_MASK_ADDR  		   0X10C4030

#define CORE0_INT1_TIMER_BIT    		      (1 << 0)
#define CORE0_INT1_NPU_BIT      		      (1 << 1)
#define CORE0_INT1_STATUS  		               0X10C4044

#define MASK_TIMER0_OUT_Pos          (7UL)
#define TIMER0_OUT_EN          		 (1UL << MASK_TIMER0_OUT_Pos)  //

#define MASK_NPU_ISR_Pos             (1UL)
#define NPU_INTERRUPT_EN   			 (1UL << MASK_NPU_ISR_Pos)  //

#define MASK_TIMER1_ISR_Pos          (0UL)
#define TIMER1_INTERRUPT_EN   		 (1UL << MASK_TIMER1_ISR_Pos)  //

//#define USE_CEVA_TIMER 1

////////////////////////CEVA REG OPERATION////////////////////////////////

//--------------------------------------------------------------------------
#define HWMEM_WORD(x) (*((volatile unsigned long *)(x)))
#define HWMEM_SHORT(x) (*((volatile unsigned short *)(x)))
#define HWMEM_CHAR(x) (*((volatile unsigned char *)(x)))


void vWriteReg128(unsigned int addr, long long	data);
void vWriteReg64(long int addr, long int data);
void vWriteReg32(unsigned int addr, unsigned int data);
void vWriteReg8(unsigned int addr,char data);

long int xReadReg64(long int addr);
unsigned int xReadReg32(unsigned int addr);
char xReadReg8(unsigned int addr);


////////////////////////CEVA LIST/////////////////////////////////////

struct AR_LIST_HEAD
{
    struct AR_LIST_HEAD *next, *prev;
};


typedef struct AR_LIST_HEAD AR_LIST_t;


static inline void vArInitListHead(struct AR_LIST_HEAD *list)
{
    list->next = list->prev = list;
}


static inline void vListAdd(struct AR_LIST_HEAD *entry,
                struct AR_LIST_HEAD *prev, struct AR_LIST_HEAD *next)
{
    next->prev = entry;
    entry->next = next;
    entry->prev = prev;
    prev->next = entry;
}

static inline void vArListAdd(struct AR_LIST_HEAD *entry, struct AR_LIST_HEAD *head)
{
    vListAdd(entry, head, head->next);
}

static inline void vArListAddTail(struct AR_LIST_HEAD *entry, struct AR_LIST_HEAD *head)
{
    vListAdd(entry, head->prev, head);
}


static inline void vListDel(struct AR_LIST_HEAD *prev, struct AR_LIST_HEAD *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void vArListDel(struct AR_LIST_HEAD *entry)
{
    vListDel(entry->prev, entry->next);
}

static inline int vArListEmpty(const struct AR_LIST_HEAD *head)
{
	return head->next == head;
}


#ifndef ar_container_of
#define ar_container_of(ptr, type, member) \
    (type *)((char *)(ptr) - (char *) &((type *)0)->member)
#endif

/**
 * Alias of container_of
 */
#define ar_list_entry(ptr, type, member) \
	   ar_container_of(ptr, type, member)

#define ar_list_first_entry(ptr, type, member) \
	ar_list_entry((ptr)->next, type, member)

#define ar_list_next_entry(pos, member) \
	ar_list_entry((pos)->member.next, typeof(*(pos)), member)

#define ar_list_for_each(pos, head) \
	for ((pos)= (head)->next; (pos)!= (head); (pos) = (pos)->next)

#define ar_list_for_each_entry_safe(pos, n, head, member)			\
            for (pos = ar_list_first_entry(head, typeof(*pos), member),    \
                n = ar_list_next_entry(pos, member);           \
                 &pos->member != (head);                    \
                 pos = n, n = ar_list_next_entry(n, member))
//////////////////////////////////////////////////////////////////////////



#define vMssEnableGlobalProgramCache() mss_enable_global_program_cache();

void vDspSystemInit(void);
void vDspApbTimerLoad(int index, unsigned int value, unsigned int enable_mode);
void vDspGetApbTimerValue(int index, unsigned int *value);
void vDspClearApbTimerInt(int index);

#endif
