/*
 * ar_callback_declare.h
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#ifndef AR_CNN_OPERATOR_DECLARE_H_
#define AR_CNN_OPERATOR_DECLARE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vec-c.h>
#include "cevaxm.h"
#include "Uart.h"
#include "mss.h"
#include "ArMalloc.h"
#include "ArNpuPlatform.h"
#include "ArCnnOperatorCommon.h"


#define VEC_C


typedef AR_S32 (*ArCnnCbFunc)(AR_U32 pIn, AR_U32 pOut, AR_NPU_CB_PARAM_S* pCBParams, void* pOpParams);

typedef struct {
    const char* pName;
    ArCnnCbFunc pCnnCbHandler;
} ArCnnOperatorDeclare_t;


#define AR_CNN_OPERATOR_DECLARE(__name)		\
	xArCnnOperatorEntryStartDeclare(ArCnnOperatorDeclare_t, __name)


#define xArCnnOperatorEntryStartDeclare(_type, _name)		\
	_type _ar_cnn_operator_declare_list_2_##_name 		\
			__attribute__((unused,			\
			section(".ar_cnn_operator_declare_list_2_"#_name)))


#define xArCnnOperatorEntryStart(_type)					\
({									\
	static char start[0]  __attribute__((unused,	\
		section(".ar_cnn_operator_declare_list_1")));			\
	(_type *)&start;						\
})

#define xArCnnOperatorEntryEnd(_type)					\
({									\
	static char end[0]  __attribute__((unused,		\
		section(".ar_cnn_operator_declare_list_3")));			\
	(_type *)&end;							\
})

#define xArCnnOperatorEntryCount(_type)					\
	({								\
		_type *start = xArCnnOperatorEntryStart(_type);		\
		_type *end = xArCnnOperatorEntryEnd(_type);		\
		unsigned int _ll_result = end - start;			\
		_ll_result;						\
	})

#define xArCnnOperatorEntryGet(_type, _name)				\
	({								\
		extern _type _ar_cnn_operator_declare_list_2_##_name;	\
		_type *_ll_result =					\
			&_ar_cnn_operator_declare_list_##_name;		\
		_ll_result;						\
	})



#endif
