/*
 * Debugging macro include header
 *
 *  Copyright (C) 2020 Artosyn
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#define UART_THR_OFFSET		0x00  /* Transmit Holding Register */
#define UART_LSR_OFFSET		0x14  /* Line Status Register */

#define UART_LSR_THRE		0x20	/* Transmit Holding Register Empty Bit */

#define UART0_PHYS		0x40500000
#define UART0_VIRT		0xF0800000

# define LL_UART_PADDR		UART0_PHYS
# define LL_UART_VADDR		UART0_VIRT

		.macro	addruart, rp, rv, tmp
		ldr	\rp, =LL_UART_PADDR	@ physical
		ldr	\rv, =LL_UART_VADDR	@ virtual
		.endm

		.macro	senduart,rd,rx
		strb	\rd, [\rx, #UART_THR_OFFSET]	@ TXDATA
		.endm

		.macro	waituart,rd,rx
1001:		ldr	\rd, [\rx, #UART_LSR_OFFSET]
		tst	\rd, #UART_LSR_THRE
		beq	1001b
		.endm

		.macro	busyuart,rd,rx
1002:		ldr	\rd, [\rx, #UART_LSR_OFFSET]	@ get status register
		tst	\rd, #UART_LSR_THRE		@
		beq	1002b			       	@ wait empty
		.endm
