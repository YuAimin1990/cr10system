#ifndef _CFG_USER_H__
#define _CFG_USER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"

/***********************************/
/***         user                ***/
/***********************************/
#define MAX_USER_NUM             8
typedef struct {
    AR_S32   enable;
    AR_CHAR    userName[MAX_STR_LEN_32];
    AR_CHAR    password[MAX_STR_LEN_32];
    AR_U32   userRight;
    AR_S32   sid;
} NET_USER_INFO;

typedef struct {
    NET_USER_INFO user[MAX_USER_NUM];
} IPC_CFG_USER_S;
typedef struct
{
    char name[8];
} BLACK_USER;
typedef struct tagBLACK_USER_NODE
{
    BLACK_USER user;
    struct tagBLACK_USER_NODE *pNext;
} BLACK_USER_NODE;
typedef struct
{
    AR_S32 num;
    BLACK_USER_NODE *h;
} IPC_CFG_BLACK_USER_S;

extern AR_S32 IPC_CFG_USER_Save();
extern AR_S32 IPC_CFG_USER_Load(CFG_RUN_MODE_E enMode);
extern AR_S32 IPC_CFG_USER_LoadDefault();
extern void IPC_CFG_USER_Print();
extern IPC_CFG_USER_S * IPC_CFG_USER_GetParam();

void IPC_CFG_BLACK_USER_Print();

extern AR_S32 IPC_CFG_BLACK_USER_Save();
extern AR_S32 IPC_CFG_BLACK_USER_Open(CFG_RUN_MODE_E enMode);
extern void IPC_CFG_BLACK_USER_Close();

#define USER_CFG_FILE "cfg_user.json"
#define BLACK_USER_CFG_FILE "black_user.json"

#ifdef __cplusplus
}
#endif
#endif

