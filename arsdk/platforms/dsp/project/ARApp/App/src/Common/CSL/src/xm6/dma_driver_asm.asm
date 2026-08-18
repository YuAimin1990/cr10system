
;    .EQU QPUSH_BASE_ADDR                    0x10C0
;    .EQU QPUSH_DSC_EXT_PTR                  QPUSH_BASE_ADDR + 0x00
;    .EQU QPUSH_DSC_INT_PTR                  QPUSH_BASE_ADDR + 0x04
;    .EQU QPUSH_DSC_TCTL                     QPUSH_BASE_ADDR + 0x08
;    .EQU QPUSH_DSC_TFRAME_LEN               QPUSH_BASE_ADDR + 0x0c
;    .EQU QPUSH_NUM                          QPUSH_BASE_ADDR + 0x10
;    .EQU EXTERNAL_QMAN_BASE_ADDR            0x402C00

    .EQU QMAN_X_OFFSET_SHIFT,                0x07
    .EQU QMAN_DESC_INC_VALUE,                0x00010001
    .EQU QMAN_QUEUE_ENABLE,                  0x80000000
    .EQU QMAN_BASE_ADDR,                     0x1184
    .EQU QMAN_ACTIVE_ADDR,                   0x1184
    .EQU QMAN_VI_MASK_ADDR,                  0x1194
    .EQU QMAN_RQ_STATUS_ADDR,                0x1198
    .EQU QMAN_RST_ADDR,                      0x119c

    .EQU INTERNAL_QMAN_BASE_ADDR,            0x1200
    .EQU QX_EN_DEPTH,                        0x1200
    .EQU QX_FIRST_ADDR,                      0x1204
    .EQU QX_BASE_PTR,                        0x1208
    .EQU QX_CHNK_SIZE,                       0x120c
    .EQU QX_DSC_EN_INC0,                     0x1210
    .EQU QX_STATUS,                          0x1214
    .EQU QX_DSC_EN_INC1,                     0x1240

    .EQU DMA_STATUS_OK,                      0x0
    .EQU DMA_STATUS_ERROR_PARAMS,            0x1
    .EQU DMA_STATUS_ALLOC_FAILED,            0x2
    .EQU DMA_STATUS_ENQUEUE_FAILED,          0x3

    .EQU MSS_DMBE_REGISTER_ADDR,         	 0x604
    .EQU MSS_DMBE_MASK_VALUE,                0x03
    .EQU MSS_BARRIER_REGISTER_ADDR,          0x638
    .EQU MSS_BARRIER_REGISTER_VALUE,         0x80


.section CSL_INTERNAL_CODE_ALLOCATION,"ax",@progbits

.if _DMA_API_CHECK_
	.global dma_enqueue_sync_point_asm
dma_enqueue_sync_point_asm:
.else
	.global dma_enqueue_sync_point
dma_enqueue_sync_point:
.endif

    sc0.mov r1.i, r4.i                                                  ; r4 - pointer of sync val

    ls0.ld (#28 + r0.ui).di,r2.ui                                          ; load queue->sync_counter

    sc1.add r0.i, #32, r1.i                                                ; prepare &queue->sync_message_desc
    || sc0.add r0.i, #24, r3.i                                             ; prepare &queue->current_sync_value

    nop #0x2

    sc0.add r2.i, #1, r2.i                                                 ; sync_counter++

    ls1.st r2.ui, (#28 + r0.ui).di                                         ; store sync_counter to queue descriptor;
    ls1.st r2.ui, (#0 + r4.ui).di                                          ; store sync_counter to input reference (*value);

.if _DMA_API_CHECK_
    .global dma_enqueue_desc_asm
dma_enqueue_desc_asm:
.else
    .global dma_enqueue_desc
dma_enqueue_desc:
.endif

;r0 queue ptr
;r1 descriptor ptr
;r2 src
;r3 dst

    ls0.ld(r0.ui).di, r4.ui                                                ; r4 - holds r_ptr

    ls0.ld (#8 + r0.ui).di4, r5.ui, r26.ui, r6.ui, r7.ui                   ; r5 w_ptr, r6 queue start address, r7 queue depth register, r26 - queue ID

	nop

    ls0.ld (r1.ui).di4, r24.ui, r45.ui, r46.ui, r27.ui                     ; r24,r45,r46,r27 hold the 1st half of the descriptor (r45 and r46 are discarded)

    ls0.ld (#16 + r1.ui).di4, r28.ui, r29.ui, r30.ui, r31.ui               ; r28 - r31 hold the 2nd half of the descriptor

    nop

    sc3.extract r7.ui, #0xD, #0x0, r45.ui                                  ; r45 - extract the QX_DEPTH field
    || sc0.add r5.ui, #32, r25.ui                                          ; r25 - the new w_ptr, incremented by the descriptor size

    sc2.shiftl r45.ui, #0x5, r45.ui                                        ; r45 - holds the queue size mask in bytes (32 bytes per descriptor)
    || ls1.st r24.i, r2.i, r3.i, r27.i, (r6.ui + r5.i).i4+#16              ; write the 1st half of the descriptor to the queue

    sc0.and r25.s2, r45.s2, r25.s2                                         ; wrap around new write_ptr
    ||  ls1.push modE.ui                                             	   ; save predicates

    sc0.cmp {neq} r25.ui, r4.ui, pr0.b2, pr1.b2                            ; compare w_ptr and r_ptr, pr0 - indicates queue is not full
    || sc1.shiftladd r26.ui, #0x07, #QX_DSC_EN_INC0, r4.ui  ; r4 - holds QX_DSC_EN_INC0 address
    || ls1.st r28.i, r29.i, r30.i, r31.i, (r6.ui + r5.i).i4+#-16           ; write the 2nd half of the descriptor to the queue

    sc0.mov #0x00010001, r3.ui                                             ; r3 - holds QX_DSC_EN_INC0 value
    || sc1.shiftladd r26.ui, #0x07, #QX_EN_DEPTH, r27.ui    ; r27 - holds QX_EN_DEPTH address

    ls1.st r25.ui, (#8 + r0.ui).ui, ?pr0.b                             	   ; update write_ptr in the queue
	|| sc0.mov #MSS_DMBE_REGISTER_ADDR, r27.ui							   ; r27 holds DMBE register address

wb_poll:
	sc0.in {cpm} (r27.ui).di, r7.di                                        ; r7 holds DMBE register value

	nop

	nop

	nop

	nop

	nop

	nop

	sc0.tst {clr} r7.di, #MSS_DMBE_MASK_VALUE, pr2.b2, pr3.b2			   ; test if write buffer is empty and there are no pending transactions on the external port

	nop

	pcu.br {ds1} #wb_poll, #0x0, #0x0,  ?pr2.b                             ; if the previous test faield - try again

	nop
;    nop

;    nop

;    sc0.mov #MSS_BARRIER_REGISTER_ADDR, r27.ui

;    sc0.mov #MSS_BARRIER_REGISTER_VALUE, r7.ui

;    nop

;    nop

;    sc0.out {cpm} r7.di, (r27.ui).di                                   	   ; write MSS internal barrier activation

;    nop

;    nop

;    nop

    sc1.mov #DMA_STATUS_OK, r0.ui, ?pr0.b                           	   ; return code on success
    || sc0.out {cpm} r3.di, (r4.ui).di, ?pr0.b                         	   ; write QX_DSC_EN_INC0
    || sc2.mov #DMA_STATUS_ENQUEUE_FAILED, r0.ui, ?pr1.b               	   ; return code on failure
    || LS0.pop modE.ui												   	   ; restore predicates - requires latency of 4 cycles

    pcu.nop
    pcu.nop
    pcu.nop
    pcu.nop
    pcu.ret {ds1}  ?pr14.b                                                 ; return has at least 2 delay slot cycles including BTB
    	pcu.nop #0x1
