/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2016 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 */
#include <linux/linkage.h>

#define SHARED_RAM_BASE			0x017c0000//0x017d0000

#define SMP_INFO_SIZE                   (0x40)
#define BOOT_INFO_STRICT_BASE_ADDR 	SHARED_RAM_BASE
#define BOOT_HOLD_ENTRY_SHIFT 		(3)
#define BOOT_SPSR_EL3_ADDR 		(BOOT_INFO_STRICT_BASE_ADDR + 24)
#define BOOT_SCR_EL3_ADDR 		(BOOT_INFO_STRICT_BASE_ADDR + 32)

#ifndef ASM_MACROS_S
#define ASM_MACROS_S

	/*
	 * This macro is used to create a function label and place the
	 * code into a separate text section based on the function name
	 * to enable elimination of unused code during linking. It also adds
	 * basic debug information to enable call stack printing most of the
	 * time. The optional _align parameter can be used to force a
	 * non-standard alignment (indicated in powers of 2). The default is
	 * _align=2 because both Aarch32 and Aarch64 instructions must be
	 * word aligned. Do *not* try to use a raw .align directive. Since func
	 * switches to a new section, this would not have the desired effect.
	 */
	.macro func _name, _align=2
	/*
	 * Add Call Frame Information entry in the .debug_frame section for
	 * debugger consumption. This enables callstack printing in debuggers.
	 * This does not use any space in the final loaded binary, only in the
	 * ELF file.
	 * Note that a function manipulating the CFA pointer location (i.e. the
	 * x29 frame pointer on AArch64) should declare it using the
	 * appropriate .cfi* directives, or be prepared to have a degraded
	 * debugging experience.
	 */
	.cfi_sections .debug_frame
	.section .text.asm.\_name, "ax"
	.type \_name, %function
	/*
	 * .cfi_startproc and .cfi_endproc are needed to output entries in
	 * .debug_frame
	 */
	.cfi_startproc
	.align \_align
	\_name:
	.endm

	/*
	 * This macro is used to mark the end of a function.
	 */
	.macro endfunc _name
	.cfi_endproc
	.size \_name, . - \_name
	.endm

	/*
	 * Helper macro to generate the best mov/movk combinations according
	 * the value to be moved. The 16 bits from '_shift' are tested and
	 * if not zero, they are moved into '_reg' without affecting
	 * other bits.
	 */
	.macro _mov_imm16 _reg, _val, _shift
		.if (\_val >> \_shift) & 0xffff
			.if (\_val & (1 << \_shift - 1))
				movk	\_reg, (\_val >> \_shift) & 0xffff, LSL \_shift
			.else
				mov	\_reg, \_val & (0xffff << \_shift)
			.endif
		.endif
	.endm

	/*
	 * Helper macro to load arbitrary values into 32 or 64-bit registers
	 * which generates the best mov/movk combinations. Many base addresses
	 * are 64KB aligned the macro will eliminate updating bits 15:0 in
	 * that case
	 */
	.macro mov_imm _reg, _val
		.if (\_val) == 0
			mov	\_reg, #0
		.else
			_mov_imm16	\_reg, (\_val), 0
			_mov_imm16	\_reg, (\_val), 16
			_mov_imm16	\_reg, (\_val), 32
			_mov_imm16	\_reg, (\_val), 48
		.endif
	.endm

	/*
	 * Macro to mark instances where we're jumping to a function and don't
	 * expect a return. To provide the function being jumped to with
	 * additional information, we use 'bl' instruction to jump rather than
	 * 'b'.
         *
	 * Debuggers infer the location of a call from where LR points to, which
	 * is usually the instruction after 'bl'. If this macro expansion
	 * happens to be the last location in a function, that'll cause the LR
	 * to point a location beyond the function, thereby misleading debugger
	 * back trace. We therefore insert a 'nop' after the function call for
	 * debug builds, unless 'skip_nop' parameter is non-zero.
	 */
	.macro no_ret _func:req, skip_nop=0
	bl	\_func
#if DEBUG
	.ifeq \skip_nop
	nop
	.endif
#endif
	.endm

#endif /* ASM_MACROS_S */

ENTRY(spin_table_secondary_jump)
.globl spin_table_reserve_begin
spin_table_reserve_begin:
	mrs	x0, mpidr_el1
	and	x0, x0, #0xff

	/* Calculate address of our hold entry */
	sub x0, x0, #1
	lsl x0, x0, #BOOT_HOLD_ENTRY_SHIFT
	mov_imm x2, BOOT_INFO_STRICT_BASE_ADDR

	/* clear mailbox address value to zero when bootup */
	str xzr, [x2, x0]

	/* Wait until we have a nonzero address */
poll_mailbox:
	wfe
	ldr	x1, [x2, x0]
	cbz	x1, 1f
	str	x0, [x2, x0]
	br	x1

1:
	b	poll_mailbox
.globl spin_table_cpu_release_addr
	.align	3
spin_table_cpu_release_addr:
	.quad	0
.globl spin_table_reserve_end
spin_table_reserve_end:
ENDPROC(spin_table_secondary_jump)
