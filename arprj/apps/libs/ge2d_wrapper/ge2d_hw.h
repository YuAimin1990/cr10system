#ifndef __GE2D_HW_H__
#define __GE2D_HW_H__
#include "osal.h"
#define GE2D_INTER_FACE2
#define GET_GE2D_ADDR_BASE (0x010Af000)

#define GE2D_REG_BASE GET_GE2D_ADDR_BASE  //need to modify later

#ifdef GE2D_INTER_FACE2
#define GE2D_IRQ_SIGNAL 166
#else
#define GE2D_IRQ_SIGNAL 161
#endif

#define GE2D_INT_END_FRAME_INT (1<<0)
#define GE2D_INT_BUFFER_DONE_INT  (1<<1)
//2-5 REVERTED
#define GE2D_INT_CMD_BUF_WRITE_FAIL_INT  (1<<6)
#define GE2D_INT_CMD_WRITE_OVERFLOW_INT  (1<<7)
#define GE2D_INT_CMD_WRITE_TIME_OUT_INT  (1<<8)
#define GE2D_INT_CANVAS_FMT_UNSUPPORTED_INT  (1<<9)
#define GE2D_INT_BMP_FMT_UNSUPPORTED_INT  (1<<10)
#define GE2D_INT_DRAW_LINE_EXCEPTION_INT  (1<<11)
#define GE2D_INT_ALFA_BLENDING_EXCEPTION_INT  (1<<12)

#define GE2D_ERR (GE2D_INT_CMD_BUF_WRITE_FAIL_INT|GE2D_INT_CMD_WRITE_OVERFLOW_INT|\
	GE2D_INT_CMD_WRITE_TIME_OUT_INT|GE2D_INT_CANVAS_FMT_UNSUPPORTED_INT|GE2D_INT_BMP_FMT_UNSUPPORTED_INT|\
	GE2D_INT_DRAW_LINE_EXCEPTION_INT|GE2D_INT_ALFA_BLENDING_EXCEPTION_INT)

typedef struct _float_axis{
    float x;
    float y;
} FLOAT_AXIS;

typedef struct _AXIS{
    int x : 16;	// x position, [-32767, 32768]
    int y : 16;	// y position, [-32767, 32768]
} AXIS;

typedef struct
{
    uint32_t GE2D_REG_00;
    uint32_t GE2D_REG_01;
    uint32_t GE2D_REG_02;
    uint32_t GE2D_REG_03;
    uint32_t GE2D_REG_04;
    uint32_t GE2D_REG_05;
    uint32_t GE2D_REG_06;
    uint32_t GE2D_REG_07;
    uint32_t GE2D_REG_08;
    uint32_t GE2D_REG_09;
    uint32_t GE2D_REG_10;
    uint32_t GE2D_REG_11;
    uint32_t GE2D_REG_12;
    uint32_t GE2D_REG_13;
    uint32_t GE2D_REG_14;
    uint32_t GE2D_REG_15;
    uint32_t GE2D_REG_16;
    uint32_t GE2D_REG_17;
    uint32_t GE2D_REG_18;
    uint32_t GE2D_REG_19;
    uint32_t GE2D_REG_20;
    uint32_t GE2D_REG_21;
    uint32_t GE2D_REG_22;
    uint32_t GE2D_REG_23;
    uint32_t GE2D_REG_24;
    uint32_t GE2D_REG_25;
    uint32_t GE2D_REG_26;
    uint32_t GE2D_REG_27;
    uint32_t GE2D_REG_28;
    uint32_t GE2D_REG_29;
    uint32_t GE2D_REG_30;
    uint32_t GE2D_REG_31;
    uint32_t GE2D_REG_32;
    uint32_t GE2D_REG_33;
    uint32_t GE2D_REG_34;
    uint32_t GE2D_REG_35;
    uint32_t GE2D_REG_36;
    uint32_t GE2D_REG_37;
    uint32_t GE2D_REG_38;
    uint32_t GE2D_REG_39;
    uint32_t GE2D_REG_40;
    uint32_t GE2D_REG_41;
    uint32_t GE2D_REG_42;
    uint32_t GE2D_REG_43;
    uint32_t GE2D_REG_44;
    uint32_t GE2D_REG_45;
    uint32_t GE2D_REG_46;
    uint32_t GE2D_REG_47;
}ge2d_reg_t;
int ge2d_hw_init();
int ge2d_hw_start_frame(frame_cmd_t *p_frame);
#if 0
void ge2d_hw_info_show(void);
#endif
int ge2d_hw_get_frame_index();
int clear_irq_force_for_debug();
#endif

