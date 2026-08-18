#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "vctrl_handler.h"
#include "vctrl_msg.h"
#include "vctrl_list.h"
#include "vctrl_venc.h"
#include "vctrl_vdec.h"

static int vctrl_set_fd = -1;

int create_socket(const char *name, int type, mode_t perm, uid_t uid,
                  gid_t gid)
{
    struct sockaddr_un addr;
    int fd, ret;

    fd = socket(PF_UNIX, type, 0);
    if (fd < 0) {
        printf("Failed to open socket '%s': %s\n", name, strerror(errno));
        return -1;
    }


    memset(&addr, 0 , sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), ARTOSYN_SOCKET_DIR"/%s",
             name);

    ret = unlink(addr.sun_path);
    if (ret != 0 && errno != ENOENT) {
        printf("Failed to unlink old socket '%s': %s\n", name, strerror(errno));
        goto out_close;
    }

    ret = bind(fd, (struct sockaddr *) &addr, sizeof (addr));
    if (ret) {
        printf("Failed to bind socket '%s': %s\n", name, strerror(errno));
        goto out_unlink;
    }

    chown(addr.sun_path, uid, gid);
    chmod(addr.sun_path, perm);

    //printf("Created socket '%s' with mode '%o', user '%d', group '%d'\n",
    //     addr.sun_path, perm, uid, gid);

    return fd;

out_unlink:
    unlink(addr.sun_path);
out_close:
    close(fd);
    return -1;
}

void start_vctrl_service(void)
{
    int fd;

    fd = create_socket(CTRL_SERVICE_NAME, SOCK_STREAM, 0666, 0, 0);
    if(fd < 0) return;
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    fcntl(fd, F_SETFL, O_NONBLOCK);

    listen(fd, 8);
    vctrl_set_fd = fd;
}

int get_vctrl_set_fd()
{
    return vctrl_set_fd;
}

void handle_vctrl_set_fd(unsigned int u32CoreClock, unsigned int u32BpuClock, unsigned int u32JpegCoreClock)
{
    vctrl_msg msg;
    int s;
    int r;
    int cr;
    struct sockaddr_un addr;
    socklen_t addr_size = sizeof(addr);
    socklen_t cr_size = sizeof(cr);
    struct pollfd ufds[1];
    const int timeout_ms = 2 * 1000;  /* Default 2 sec timeout for caller to send vctrl. */
    int nr;

    if ((s = accept(vctrl_set_fd, (struct sockaddr *) &addr, &addr_size)) < 0) {
        return;
    }

    /* Check socket options here */
    if (getsockopt(s, SOL_SOCKET, SO_PEERCRED, &cr, &cr_size) < 0) {
        close(s);
        printf("Unable to receive socket options\n");
        return;
    }

    ufds[0].fd = s;
    ufds[0].events = POLLIN;
    ufds[0].revents = 0;
    nr = TEMP_FAILURE_RETRY(poll(ufds, 1, timeout_ms));
    if (nr == 0) {
        printf("sys_vctrl: timeout waiting for uid=%d to send vctrl message.\n", cr);
        close(s);
        return;
    } else if (nr < 0) {
        printf("sys_vctrl: error waiting for uid=%d to send vctrl message. err=%d %s\n", cr, errno, strerror(errno));
        close(s);
        return;
    }

    r = TEMP_FAILURE_RETRY(recv(s, &msg, sizeof(msg), MSG_DONTWAIT));

    //printf("%s received size msg=%ld, size vctrl_msg=%ld\n",__FILE__,sizeof(msg),sizeof(vctrl_msg));
    if (r != sizeof(vctrl_msg)) {
        printf("sys_vctrl: mis-match msg size received: %d expected: %zu errno: %d\n",
              r, sizeof(vctrl_msg), errno);
        close(s);
        return;
    }

    VCHN_NODE inChnNode;
    memset(&inChnNode, 0, sizeof(VCHN_NODE));
    VCTRL_SEND *pCtlSend = &msg.send_data;
    VCHN_NODE *pHandleVchn = NULL;

    switch(msg.cmd) {
    case VCTRL_MSG_START:
        close(s);
        inChnNode.chnId = pCtlSend->chnId;
        VCHN_INFO* pChnInfo = &inChnNode.vchnInfo;
        pChnInfo->s32Verbose = pCtlSend->verbose;
        pChnInfo->processor = pCtlSend->processor;
        pChnInfo->chnId = pCtlSend->chnId;
        pChnInfo->transcodingChnId = pCtlSend->transcodingChnId;
        pChnInfo->enPayLoad = pCtlSend->type;
        pChnInfo->enTranscodingPayLoad = pCtlSend->transcodingType;
        pChnInfo->u32Width = pCtlSend->width;
        pChnInfo->u32Height= pCtlSend->height;
        pChnInfo->enRcMode = pCtlSend->enRcMode;
        pChnInfo->u32Gop = pCtlSend->gopsize;
        pChnInfo->u32PicNum = pCtlSend->picNum;
        pChnInfo->u32ChnNum = pCtlSend->u32ChnNum;
        if (pCtlSend->preLoadPicNum > MAX_PRELOAD_PIC_NUM) {
            pCtlSend->preLoadPicNum = MAX_PRELOAD_PIC_NUM;
        }
        pChnInfo->preLoadPicNum = pCtlSend->preLoadPicNum;
        pChnInfo->u32BitRate = pCtlSend->bitrate;
        pChnInfo->u32FrameRate = pCtlSend->frameRate;
        pChnInfo->u32SrcFrameRate = pCtlSend->srcframeRate;
        if (pChnInfo->u32SrcFrameRate == 0) {
            pChnInfo->u32SrcFrameRate = pChnInfo->u32FrameRate;
        }
        pChnInfo->transcoding = pCtlSend->transcoding;
        if (pChnInfo->preLoadPicNum > 0) {
            pChnInfo->preLoadPicFlag = 1;
        }

        pChnInfo->s32SaveFile = pCtlSend->s32SaveFile;
        pChnInfo->u32CoreClock = u32CoreClock;
        pChnInfo->u32BpuClock = u32BpuClock;
        pChnInfo->u32JpegCoreClock = u32JpegCoreClock;

        if (VIDEO_ENCODER == pChnInfo->processor) {
            memcpy(pChnInfo->inputFilePath, pCtlSend->yuvFile,strlen(pCtlSend->yuvFile));
            memcpy(pChnInfo->outputFilePath, pCtlSend->streamFile,strlen(pCtlSend->streamFile));

            inChnNode.vcodec_start = VCTRL_VENC_SERVICE_START;
            inChnNode.vcodec_stop = VCTRL_VENC_SERVICE_STOP;
            inChnNode.vcodec_set = VCTRL_VENC_SERVICE_SET;
        } else if (VIDEO_DECODER == pChnInfo->processor) {
            memcpy(pChnInfo->inputFilePath, pCtlSend->streamFile,strlen(pCtlSend->streamFile));
            memcpy(pChnInfo->outputFilePath, pCtlSend->yuvFile,strlen(pCtlSend->yuvFile));

            inChnNode.vcodec_start = VCTRL_VDEC_SERVICE_START;
            inChnNode.vcodec_stop = VCTRL_VDEC_SERVICE_STOP;
            inChnNode.vcodec_set = VCTRL_VDEC_SERVICE_SET;
        }

        printf("[server] VCTRL_MSG_START chn%d pCtlSend = %p\n", pCtlSend->chnId, pCtlSend);
        queue_vchnnel_list(&inChnNode);

        VCTRL_PRT("chn%d, mode = %s, codec = %d, width = %d, height = %d, enRcMode = %d, gopsize = %d, bitrate = %d, frameRate = %d\n",
                   pChnInfo->chnId, (pChnInfo->processor == VIDEO_ENCODER) ? "video_encoder" : "video_decoder", pChnInfo->enPayLoad, pChnInfo->u32Width,
                   pChnInfo->u32Height, pChnInfo->enRcMode, pChnInfo->u32Gop, pChnInfo->u32BitRate, pChnInfo->u32FrameRate);
        VCTRL_PRT("inputFilePath = %s, outputFilePath = %s\n", pChnInfo->inputFilePath, pChnInfo->outputFilePath);
        start_vchnnel_work();
        break;
    case VCTRL_MSG_STOP:
        printf("[server] VCTRL_MSG_STOP chn%d pCtlSend = %p\n", pCtlSend->chnId, pCtlSend);
        stop_vchnnel_work(pCtlSend->chnId);
        close(s);
        break;
    case VCTRL_MSG_SET:
        VCTRL_PRT("get msg VCTRL_MSG_SET\n");
        pHandleVchn = get_venchn_by_id((AR_S32)pCtlSend->chnId);

        if(pHandleVchn != NULL) {
            VCHN_INFO* pChnInfo = &pHandleVchn->vchnInfo;
            if (NULL != pHandleVchn->vcodec_set) {
                pChnInfo->processor = pCtlSend->processor;
                pChnInfo->chnId = pCtlSend->chnId;
                pChnInfo->enPayLoad = pCtlSend->type;
                pChnInfo->u32Width = pCtlSend->width;
                pChnInfo->u32Height= pCtlSend->height;
                pChnInfo->enRcMode = pCtlSend->enRcMode;
                pChnInfo->u32Gop = pCtlSend->gopsize;
                pChnInfo->u32PicNum = pCtlSend->picNum;
                pChnInfo->u32ChnNum = pCtlSend->u32ChnNum;
                pChnInfo->u32BitRate = pCtlSend->bitrate;
                pChnInfo->u32FrameRate = pCtlSend->frameRate;
                pChnInfo->transcoding = pCtlSend->transcoding;
                pChnInfo->s32SaveFile = pCtlSend->s32SaveFile;
                pChnInfo->enUpdate = pCtlSend->enUpdate;
                memcpy(&pChnInfo->roiMap, &pCtlSend->roiMap , sizeof(ROI_MAP));
                memcpy(&pChnInfo->qpChange, &pCtlSend->qpChange , sizeof(QP_CHANGE));
                pHandleVchn->vcodec_set(pChnInfo);
            } else {
                VCTRL_PRT("Not found set chn id %d\n", pCtlSend->chnId);
            }
        }
        close(s);
        break;
    default:
        close(s);
        break;
    }
}

