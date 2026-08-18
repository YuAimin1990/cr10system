;
; Copyright (C) CEVA(R) Inc. All rights reserved
;
;
; This product or Software is made available exclusively to licensees that have
; received express written authorization from CEVA to download or receive the product
; or Software and have agreed to the End User License Agreement (EULA).  If you have
; not received such express authorization and agreed to the CEVA EULA, you may not
; download, install or use this product or Software.
;
; The information contained in this document is subject to change without notice and
; does not represent a commitment on any part of CEVA(R), Inc. CEVA(R), Inc. and its
; subsidiaries make no warranty of any kind with regard to this material, including,
; but not limited to implied warranties of merchantability and fitness for a
; particular purpose whether arising out of law, custom, conduct or otherwise.
;
; While the information contained herein is assumed to be accurate, CEVA(R), Inc.
; assumes no responsibility for any errors or omissions contained herein, and
; assumes no liability for special, direct, indirect or consequential damage,
; losses, costs, charges, claims, demands, fees or expenses, of any nature or kind,
; which are incurred in connection with the furnishing, performance or use of this
; material.
;
; This document contains proprietary information, which is protected by U.S. and
; international copyright laws. All rights reserved. No part of this document may be
; reproduced, photocopied, or translated into another language without the prior
; written consent of CEVA(R), Inc.
;

.EQU XM6_SAVE_VECTORS_IN_CONTEXT,   1

.EXTERN pxCurrentTCB
.EXTERN vPortSysTickHandler
.EXTERN vTaskSwitchContext
; artosyn interrupt handler
.EXTERN vPortint1NPUHandler
.EXTERN vIccIsr
.EXTERN vPortMuxIrqHandler

.section RTOS_CODE,"ax",@progbits
;----------------------------------------------------------
.global portYIELD
portYIELD:
	brr {ds2} #common, #0x0, #0x0, ?pr14.b
	|| push {auxreg9}
		push {auxreg8}
		|| mov #vTaskSwitchContext, r63.ui
		push {auxreg7}
;----------------------------------------------------------
.global vPortTickInterrupt
;time0 / int0  for rtos tick
vPortTickInterrupt:
	brr {ds2} #common, #0x0, #0x0, ?pr14.b
	|| push {auxreg9}
		push {auxreg8}
		|| mov #vPortSysTickHandler, r63.ui
		push {auxreg7}
;-----------------------------------------------------------
.global vPortNPUInterrupt
;int1 for npu interrupt
vPortNPUInterrupt:
	brr {ds2} #common, #0x0, #0x0, ?pr14.b
	|| push {auxreg9}
		push {auxreg8}
		|| mov #vPortint1NPUHandler, r63.ui
		push {auxreg7}
;------------------------------------------------------------
.global vPortint2Interrupt
;int2 for other interrupt
vPortint2Interrupt:
	;Store DSP regs
	brr {ds2} #common, #0x0, #0x0, ?pr14.b
	|| push {auxreg9}
		push {auxreg8}
		|| mov #vIccIsr, r63.ui
		push {auxreg7}
;------------------------------------------------------------
.global vPortMuxInterrupt
;int1 is a mux of npu and ext timer interrupt
vPortMuxInterrupt:
	brr {ds2} #common, #0x0, #0x0, ?pr14.b
	|| push {auxreg9}
		push {auxreg8}
		|| mov #vPortMuxIrqHandler, r63.ui
		push {auxreg7}
;------------------------------------------------------------
common:
	push {auxreg6}
	push {auxreg5}
	push {auxreg4}
	push {auxreg3}
	push {auxreg2}
	push {auxreg10}
	push {auxreg1}
	|| ld (#pxCurrentTCB).ui, r0.ui
	push {auxreg0}
	;|| mov sp.ui, r62.ui
	;push {in1} r62.ui

.if XM6_SAVE_VECTORS_IN_CONTEXT==1
	vpush v47.i8
	vpush v46.i8
	vpush v45.i8
	vpush v44.i8
	vpush v43.i8
	vpush v42.i8
	vpush v41.i8
	vpush v40.i8
	vpush v39.i8
	vpush v38.i8
	vpush v37.i8
	vpush v36.i8
	vpush v35.i8
	vpush v34.i8
	vpush v33.i8
	vpush v32.i8
	vpush v31.i8
	vpush v30.i8
	vpush v29.i8
	vpush v28.i8
	vpush v27.i8
	vpush v26.i8
	vpush v25.i8
	vpush v24.i8
	vpush v23.i8
	vpush v22.i8
	vpush v21.i8
	vpush v20.i8
	vpush v19.i8
	vpush v18.i8
	vpush v17.i8
	vpush v16.i8
	vpush v15.i8
	vpush v14.i8
	vpush v13.i8
	vpush v12.i8
	vpush v11.i8
	vpush v10.i8
	vpush v9.i8
	vpush v8.i8
	vpush v7.i8
	vpush v6.i8
	vpush v5.i8
	vpush v4.i8
	vpush v3.i8
	vpush v2.i8
	vpush v1.i8
	vpush v0.i8
	nop
.else
	nop #0x2
.endif

	mov sp.ui, r61.ui
	;Call to increment timer tick variable and switch context if needed
	PCU.callar {ds1} r63.i, ?pr14.b
	st r61.ui, (r0.ui).ui ; store sp of current task in its TCB
	
.global restore_sp_from_pxCurrentTCB
	restore_sp_from_pxCurrentTCB:
	;restore TCB of the new task
	; restore stack pointer
	ld (#pxCurrentTCB).ui, r0.ui
	nop
	nop
	nop
	ld (r0.ui).ui, r1.ui
	nop
	nop
	nop
	nop
	nop
	mov r1.ui, sp.ui
	nop
	nop
	nop
	nop
	nop
	nop
	
	; restore reigsters of the new task
.if XM6_SAVE_VECTORS_IN_CONTEXT==1
	vpop v0.i8
	vpop v1.i8
	vpop v2.i8
	vpop v3.i8
	vpop v4.i8
	vpop v5.i8
	vpop v6.i8
	vpop v7.i8
	vpop v8.i8
	vpop v9.i8
	vpop v10.i8
	vpop v11.i8
	vpop v12.i8
	vpop v13.i8
	vpop v14.i8
	vpop v15.i8
	vpop v16.i8
	vpop v17.i8
	vpop v18.i8
	vpop v19.i8
	vpop v20.i8
	vpop v21.i8
	vpop v22.i8
	vpop v23.i8
	vpop v24.i8
	vpop v25.i8
	vpop v26.i8
	vpop v27.i8
	vpop v28.i8
	vpop v29.i8
	vpop v30.i8
	vpop v31.i8
	vpop v32.i8
	vpop v33.i8
	vpop v34.i8
	vpop v35.i8
	vpop v36.i8
	vpop v37.i8
	vpop v38.i8
	vpop v39.i8
	vpop v40.i8
	vpop v41.i8
	vpop v42.i8
	vpop v43.i8
	vpop v44.i8
	vpop v45.i8
	vpop v46.i8
	vpop v47.i8
.else
	nop
	nop
	nop
	nop
	nop
.endif
	pop {auxreg0}
	pop {auxreg1}
	pop {auxreg10}
	pop {auxreg2}
	pop {auxreg3}
	pop {auxreg4}
	pop {auxreg5}
	pop {auxreg6}
	pop {auxreg7}
	pop {auxreg8}
	pop {auxreg9}
	nop
	reti

;This function store the current DSP regs for each task. This is done only in the initialization.
;When task wakes up for the first time its regs will have these values.
.global initTcbStackRegs
initTcbStackRegs:
	push retreg.ui        ; this is not a part of the saved context - this push goes to the main stack rather than the task's stack
	mov sp.ui, r7.ui
	ld (r1.ui).ui, r6.ui
	nop
	nop
	nop
	nop
	nop
	nop
	mov r6.ui, sp.ui
	nop
	nop
	nop
	nop
	nop
	nop
	push {auxreg9}
	push {auxreg8}
	push {auxreg7}
	push {auxreg6}
	push {auxreg5}
	push {auxreg4}
	push {auxreg3}
	push {auxreg2}
	mov sp.ui, r3.ui                      ;save the context address of r0 (arguments to task)
	st r3.ui, (r0.ui).di                ;return the r0 context address to argument #0
	push {auxreg10}
	push {auxreg1}
	push {auxreg0}
	mov sp.ui, r3.ui                      ;save the context address of moda register
	st r3.ui, (r2.ui).di                ;return the moda register context address to argument #2

.if XM6_SAVE_VECTORS_IN_CONTEXT==1
	vpush v47.i8
	vpush v46.i8
	vpush v45.i8
	vpush v44.i8
	vpush v43.i8
	vpush v42.i8
	vpush v41.i8
	vpush v40.i8
	vpush v39.i8
	vpush v38.i8
	vpush v37.i8
	vpush v36.i8
	vpush v35.i8
	vpush v34.i8
	vpush v33.i8
	vpush v32.i8
	vpush v31.i8
	vpush v30.i8
	vpush v29.i8
	vpush v28.i8
	vpush v27.i8
	vpush v26.i8
	vpush v25.i8
	vpush v24.i8
	vpush v23.i8
	vpush v22.i8
	vpush v21.i8
	vpush v20.i8
	vpush v19.i8
	vpush v18.i8
	vpush v17.i8
	vpush v16.i8
	vpush v15.i8
	vpush v14.i8
	vpush v13.i8
	vpush v12.i8
	vpush v11.i8
	vpush v10.i8
	vpush v9.i8
	vpush v8.i8
	vpush v7.i8
	vpush v6.i8
	vpush v5.i8
	vpush v4.i8
	vpush v3.i8
	vpush v2.i8
	vpush v1.i8
	vpush v0.i8
.endif
	nop #0x2

	mov r7.ui, sp.ui
	|| mov sp.ui, r3.ui                   ;save new top of stack
	st r3.ui, (r1.ui).di                ;return top of stack address to argument #1
	nop
	nop
	nop
	nop
	nop
	pop retreg.ui
	nop
	nop
	nop
	ret {ds1} ?pr14.b
	nop

