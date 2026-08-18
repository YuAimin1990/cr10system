#ifndef __IPC_VGS_H__
#define __IPC_VGS_H__

#include "hal_region.h"
#include "hal_dbglog.h"
#include "mpi_vb.h"
#include "mpi_vgs.h"
#include "osal.h"


//#define ENABLE_VGS 1

#define AR_ALIGN4(_x)              (((_x)+0x03)&~0x03)
#define AR_ALIGN256(_x)            (((_x)+0xff)&~0xff)

typedef struct
{
    AR_VOID  *y_vrt;
    AR_VOID  *u_vrt;
    AR_VOID  *v_vrt;

    AR_VOID  *y_phy;
    AR_VOID  *u_phy;
    AR_VOID  *v_phy;

    VB_BLK    y_blk;
    VB_BLK    u_blk;
    VB_BLK    v_blk;

    AR_S32    y_stride;
    AR_S32    uv_stride;
    AR_S32    w;
    AR_S32    h;
}STRU_ADDR;

typedef struct
{
    unsigned char type[2];
    unsigned char size[4];
    unsigned char retain[4];
    unsigned char offset[4];
    unsigned char head_len[4];
    unsigned char width[4];
    unsigned char height[4];
    unsigned char planes[2];
    unsigned char bitCount[2];
}bmp_head_t;

int  IPC_VGS_Init(void);
void IPC_VGS_UnInit();

#endif
