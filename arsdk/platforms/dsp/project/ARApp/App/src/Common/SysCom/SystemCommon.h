/*
 * system_common.h
 *
 *  Created on: 2017Äê4ÔÂ27ÈÕ
 *      Author: jianliu
 */

#ifndef SYSTEMCOMMON_H_
#define SYSTEMCOMMON_H_
#ifdef __cplusplus
extern "C"{
#endif

extern void vBlockInstructuon(int instrucments)__attribute__((section(".CSECT internal_code_sec")));

#ifdef __cplusplus
}
#endif
#endif /* SYSTEM_COMMON_H_ */
