/*
 * ar_callback_declare.h
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#ifndef AR_CNN_PARSE_DECLARE_H_
#define AR_CNN_PARSE_DECLARE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cevaxm.h"
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "Uart.h"
#include "ArNpuPlatform.h"
//#include "ArCevaArtSimulator.h"


typedef AR_S32 (*ArCnnParseFunc)(const void* JsonNode, AR_U32 u32JsonLen, void** pOpParams, AR_U32 *pOpParamLen);


typedef struct {
	const char* pName;
    ArCnnParseFunc pCnnParseHandler;
} ArCnnParseDeclare_t;


#define AR_CNN_PARSE_DECLARE(__name)		\
	xArCnnParseEntryStartDeclare(ArCnnParseDeclare_t, __name)


#define xArCnnParseEntryStartDeclare(_type, _name)		\
	_type _ar_cnn_parse_declare_list_2_##_name 		\
			__attribute__((unused,			\
			section(".ar_cnn_parse_declare_list_2_"#_name)))


#define xArCnnParseEntryStart(_type)					\
({									\
	static char start[0]  __attribute__((unused,	\
		section(".ar_cnn_parse_declare_list_1")));			\
	(_type *)&start;						\
})

#define xArCnnParseEntryEnd(_type)					\
({									\
	static char end[0]  __attribute__((unused,		\
		section(".ar_cnn_parse_declare_list_3")));			\
	(_type *)&end;							\
})

#define xArCnnParseEntryCount(_type)					\
	({								\
		_type *start = xArCnnParseEntryStart(_type);		\
		_type *end = xArCnnParseEntryEnd(_type);		\
		unsigned int _ll_result = end - start;			\
		_ll_result;						\
	})

#define xArCnnParseEntryGet(_type, _name)				\
	({								\
		extern _type _ar_cnn_parse_declare_list_2_##_name;	\
		_type *_ll_result =					\
			&_ar_cnn_parse_declare_list_##_name;		\
		_ll_result;						\
	})



#endif
