/*
 * ar_callback_declare.h
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#ifndef AR_ARM_REMOTECALL_DECLARE_H_
#define AR_ARM_REMOTECALL_DECLARE_H_

#include <stdlib.h>
#include <string.h>
#include "cevaxm.h"
#include "Uart.h"
#include "ArNpuPlatform.h"
//#include "ArTasksSchedule.h"

typedef AR_S32 (*ArRmtCallFunc)(void *pParams);

typedef struct {
    const char* pName;
    ArRmtCallFunc pRmtCallHandler;
} ArARMRemoteCallDeclare_t;


#define AR_ARM_REMOTECALL_DECLARE(__name)		\
	xArArmRemoteCallEntryStartDeclare(ArARMRemoteCallDeclare_t, __name)


#define xArArmRemoteCallEntryStartDeclare(_type, _name)		\
	_type _ar_arm_remotecall_declare_list_2_##_name 		\
			__attribute__((unused,			\
			section(".ar_arm_remotecall_declare_list_2_"#_name)))


#define xArArmRemoteCallEntryStart(_type)					\
({									\
	static char start[0]  __attribute__((unused,	\
		section(".ar_arm_remotecall_declare_list_1")));			\
	(_type *)&start;						\
})

#define xArArmRemoteCallEntryEnd(_type)					\
({									\
	static char end[0]  __attribute__((unused,		\
		section(".ar_arm_remotecall_declare_list_3")));			\
	(_type *)&end;							\
})

#define xArArmRemoteCallEntryCount(_type)					\
	({								\
		_type *start = xArArmRemoteCallEntryStart(_type);		\
		_type *end = xArArmRemoteCallEntryEnd(_type);		\
		unsigned int _ll_result = end - start;			\
		_ll_result;						\
	})

#define xArArmRemoteCallEntryGet(_type, _name)				\
	({								\
		extern _type _ar_arm_remotecall_declare_list_2_##_name;	\
		_type *_ll_result =					\
			&_ar_arm_remotecall_declare_list_##_name;		\
		_ll_result;						\
	})



#endif
