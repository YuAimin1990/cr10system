/*****************************************************************************\
* CEVA Confidential property.
* Copyright (C) Ceva Inc. All rights reserved.
*
* This file constitutes proprietary and confidential information of CEVA Inc.
* Any use or copying of this file or any information contained in this file other
* than as expressly approved in writing by Ceva, Inc. is strictly prohibited.
* Any disclosure or distribution of this file or any information contained in
* this file except to the intended recipient is strictly prohibited.
\*****************************************************************************/

#include "cevaxm.h"
#include "ocem.h"
#include "bitops.h"

void prof_counter_config(prof_counter_id_e    counter_id,
                         prof_counter_event_e counter_event,
                         unsigned int         low_address,
                         unsigned int         high_address)
{
    unsigned int val;
    unsigned int addr;
    unsigned int offset;

    if (high_address != 0 && low_address <= high_address)
    {
        // set address range - low address[31:1]
        cpm_out(OCEM_PROF_LOW_ADD0_ADDR + (counter_id * 4), (low_address  & ~1U)     );
        // set address range - high address[31:1], enable range (P_EN = 1)
        cpm_out(OCEM_PROF_HI_ADD0_ADDR  + (counter_id * 4), (high_address & ~1U) | 1 );
    }
    else
    {
        // disable range check (P_EN = 0)
        cpm_out(OCEM_PROF_HI_ADD0_ADDR + (counter_id * 4), 0);
    }

    // set event selection in PROF_CTRL0/1 registers (each register holds configuration of four counters):
    addr = OCEM_PROF_CTRL0_ADDR;
    if (counter_id >= PROF_COUNTER_CNT4)
    {
        addr = OCEM_PROF_CTRL1_ADDR;
        counter_id -= PROF_COUNTER_CNT4;
    }

    val = cpm_in(addr);
    offset = counter_id << 3;
    BF_SET(val, counter_event, offset, 5);
    cpm_out(addr, val);
}
