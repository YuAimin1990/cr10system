#ifndef __RECMNG_SD_H__
#define __RECMNG_SD_H__

#include "util_common.h"
#include "util_api.h"

#ifdef __cplusplus
extern "C" {
#endif

//hbbai: we record to /tmp for test.
#define IPC_SD_PATHNAME_HEADER      "/dev/mmcblk"
#define IPC_SD_PATHNAME             "/dev/mmcblk[1-9]"
#define IPC_SD_PARTITION_PATHNAME   "/dev/mmcblk[1-9]p[0-9]"
#define IPC_SD_MOUNT_POINT          "/sd/"
#define IPC_SD_PATHNAME_0           "/dev/mmcblk1"
#define IPC_SD_PARTITION_PATHNAME_0 "/dev/mmcblk1p1"

#define IPC_SD_VER_FILE            "/sd/video/Version.txt"

#define IPC_SD_VIDEO_PATH             "/sd/video"
#define IPC_SD_SNAP_PATH             "/sd/photo"

typedef enum {
        IPC_SD_DISKTYPE_MMC = 0,
        IPC_SD_DISKTYPE_SDA,
} IPC_SD_DISKTYPE;

typedef enum {
        IPC_SD_FSTYPE_FAT32 = 0,
        IPC_SD_FSTYPE_NTFS,
        IPC_SD_FSTYPE_REISERFS,
        IPC_SD_FSTYPE_EXTENDED,
        IPC_SD_FSTYPE_UNKNOWN,
} IPC_SD_FSTYPE;

enum{
    IPC_SD_STATUS_OK        = 0,
    IPC_SD_STATUS_NOTEXIST  = -1,
    IPC_SD_STATUS_NO_SPACE  = -2,
    IPC_SD_STATUS_ERROR  = -3,
};

typedef enum
{
    SD_STATUS_IS_INSERT_OFFSET = 0,
    SD_STATUS_IS_WRITABLE_OFFSET = 1,
    SD_STATUS_IS_FREE_OFFSET = 2,
    SD_STATUS_IS_REC_OFFSET = 3,
    SD_STATUS_IS_SNAP_OFFSET = 4,
    SD_STATUS_IS_FORMATTING_OFFSET = 5
} E_SD_STATUS_OFFSET;

AR_S32 is_fix_format_file(AR_CHAR *str);
AR_S32 is_fix_format_path(AR_CHAR *file_path);
AR_S32 is_date_dir_format(AR_CHAR *str);
AR_S32 is_avi_file_path_format(AR_CHAR *str);
AR_S32 is_video_file_format(AR_CHAR *str, AR_S32 type);
AR_S32 is_avi_file_format(AR_CHAR *str);
AR_S32 is_mov_file_format(AR_CHAR *str);
AR_S32 is_snap_file_format(AR_CHAR *str);
AR_S32 IPC_MID_RECMNG_MountSD(void);
AR_S32 IPC_MID_RECMNG_RemountSD(void);
AR_S32 IPC_MID_RECMNG_UmountSD(void);
AR_S32 IPC_MID_RECMNG_IsSDMount(void); // if sd card is mounted, return 1; else return 0

AR_S32 IPC_MID_RECMNG_SetSDName(void);
AR_S32 IPC_MID_RECMNG_FormatSD(void);
AR_S32 IPC_MID_RECMNG_GetFormatProgress();
AR_S32 IPC_MID_RECMNG_GetSDFreeSize(void);
AR_S32 IPC_MID_RECMNG_GetSDSize(void);
AR_S32 IPC_MID_RECMNG_InitSD(void);
AR_S32 IPC_MID_RECMNG_DelOldestDir(void);
AR_S32 IPC_MID_RECMNG_IsSDExist(void);

void IPC_MID_RECMNG_UpdateSDStatus();
AR_S32 IPC_MID_RECMNG_GetSDStatus();
AR_S32 IPC_MID_RECMNG_SDStatusIs(E_SD_STATUS_OFFSET offset);
void IPC_MID_RECMNG_SDStatusSet(E_SD_STATUS_OFFSET offset);
void IPC_MID_RECMNG_SDStatusClr(E_SD_STATUS_OFFSET offset);

#ifdef __cplusplus
}
#endif

#endif /* __RECMNG_SD_H__ */

