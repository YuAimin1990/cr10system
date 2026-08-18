#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/un.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <getopt.h>
#include "vctrl_client.h"
#include "vctrl_msg.h"
#include "ar_comm_video.h"

#define logline(fmt, ...) printf("[%s:%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

int roiIndex = 0, roiIndexEnable = 0, roiEnable = 0, roiQp = 0;
int roiStartX = 0, roiStartY = 0, roiWidth = 0, roiHeigth = 0, roiUpdateFlag = 0;
int minIQp = 0, maxIQp = 0, minPQp = 0, maxPQp = 0, minBQp = 0, maxBQp = 0;
int minIprop = 0, maxIprop = 0, fixedIQp =0, fixedPQp = 0, fixedBQp = 0;
int firstFrameStartQp = 0, minQfactor = 0, maxQfactor = 0;
int transCoding = 0, preLoadPicNum = 0;

static struct option long_options[] = {
    {"verbose", no_argument, NULL, 'v'},
    {"bind", no_argument, NULL, 'B'},
    {"channel", required_argument, NULL, 'c'},
    {"transcodeChn", required_argument, NULL, 'C'},
    {"frameRate", required_argument, NULL, 'f'},
    {"srcframeRate", required_argument, NULL, 'F'},
    {"picNum", required_argument, NULL, 'n'},
    {"chnNum", required_argument, NULL, 'N'},
    {"type", required_argument, NULL, 't'},
    {"transcodeType", required_argument, NULL, 'T'},
    {"height", required_argument, NULL, 'h'},
    {"width", required_argument, NULL, 'w'},
    {"bitrate", required_argument, NULL, 'b'},
    {"gopsize", required_argument, NULL, 'g'},
    {"input", required_argument, NULL, 'i'},
    {"output", required_argument, NULL, 'o'},
    {"mode", required_argument, NULL, 'm'},
    {"rcMode", required_argument, NULL, 'M'},
    {"runCmd", required_argument, NULL, 'R'},
    {"roiIndex", required_argument, &roiIndex, 1},
    {"roiIndexEnable", required_argument, &roiIndexEnable, 1},
    {"roiQp", required_argument, &roiQp, 1},
    {"roiStartX", required_argument, &roiStartX, 1},
    {"roiStartY", required_argument, &roiStartY, 1},
    {"roiWidth", required_argument, &roiWidth, 1},
    {"roiHeigth", required_argument, &roiHeigth, 1},
    {"minIQp", required_argument, &minIQp, 1},
    {"maxIQp", required_argument, &maxIQp, 1},
    {"minPQp", required_argument, &minPQp, 1},
    {"maxPQp", required_argument, &maxPQp, 1},
    {"minBQp", required_argument, &minBQp, 1},
    {"maxBQp", required_argument, &maxBQp, 1},
    {"minIprop", required_argument, &minIprop, 1},
    {"maxIprop", required_argument, &maxIprop, 1},
    {"fixedIQp", required_argument, &fixedIQp, 1},
    {"fixedPQp", required_argument, &fixedPQp, 1},
    {"fixedBQp", required_argument, &fixedBQp, 1},
    {"firstFrameStartQp", required_argument, &firstFrameStartQp, 1},
    {"minQfactor", required_argument, &minQfactor, 1},
    {"maxQfactor", required_argument, &maxQfactor, 1},
    {"transCoding", required_argument, &transCoding, 1},
    {"preLoadPicNum", required_argument, &preLoadPicNum, 1},
    {"help", no_argument, NULL, 'H'},
    {NULL, 0, NULL, 0}
};

static const char vctrl_service_socket[]="/tmp/vctrl_service";

static int send_vctrl_msg(const vctrl_msg *msg)
{
    const int fd = socket(AF_LOCAL, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (fd == -1) {
        return -1;
    }

    const size_t namelen = strlen(vctrl_service_socket);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    strncpy(addr.sun_path, vctrl_service_socket, sizeof(addr.sun_path));
    addr.sun_family = AF_LOCAL;
    socklen_t alen = namelen + offsetof(struct sockaddr_un, sun_path) + 1;
    if (TEMP_FAILURE_RETRY(connect(fd, (struct sockaddr*) &addr, alen)) < 0) {
        VCTRL_PRT("connect failed, %d\n", alen);
        close(fd);
        return -1;
    }

    const int num_bytes = TEMP_FAILURE_RETRY(send(fd, msg, sizeof(vctrl_msg), 0));

    int result = -1;
    if (num_bytes == sizeof(vctrl_msg)) {
        struct pollfd pollfds[1];
        pollfds[0].fd = fd;
        pollfds[0].events = 0;
        const int poll_result = TEMP_FAILURE_RETRY(poll(pollfds, 1, 250 /* ms */));
        if (poll_result == 1 && (pollfds[0].revents & POLLHUP) != 0) {
            result = 0;
        } else {
            result = 0;
        }
    }
    //printf("send num_bytes %d, result %d!\n",num_bytes, result);
    close(fd);
    return result;
}

static void usage(void)
{
    fprintf(stderr,
        "Usage: test_mpp_venc [OPTION]\n"
        "\n"
        " -v, --verbose        display more detail information\n"
        " -B, --bind           use bind\n"
        " -c, --channel        venc channel: 0 - 23, default 0\n"
        " -f, --frameRate      default 25\n"
        " -n, --picNum         picture numbers, default -1\n"
        " -N, --ChnNum         chnnel numbers, default 1\n"
        " -l, --save           save the output file: 0:no save; 1:save\n"
        " -t, --type           video type: h265, h264, jpeg, mjpeg, default h265\n"
        " -h, --height         default 1080\n"
        " -w, --width          default 1920\n"
        " -b, --bitrate        default 4096 kbps\n"
        " -g, --gopsize        default 10\n"
        " -i, --input          yuv file, default /tmp/test_rc.yuv\n"
        " -o, --output         output stream file, default /mnt/debug.hevc\n"
        " -m, --mode           encoder, decoder or encoder + decoder\n"
        " -M, --rcMode         0: CBR, 1: VBR, 2: AVBR, 3: QPMAP, 4: FIXQP, default 1\n"
        " -H, --help           print this help\n"
        " -R, --start stop set default 1:start 2:stop 3:set\n"
        "\n"
        "example:\n"
        " vctrl_client -v       display more detail\n"
        " vctrl_client -c 1 -t h264 -g 20   use venc channel 1, video type: h264, gopsize: 20\n"
        "\n");
}

int main(int argc, char **argv)
{
    AR_S32 c;

    VCTRL_SEND send_data;
    memset(&send_data, 0, sizeof(send_data));
    AR_CHAR inputFile[FILE_PATH_MAX] = {0}, outputFile[FILE_PATH_MAX] = {0};

    send_data.chnId = 0;
    send_data.picNum = -1;
    send_data.u32ChnNum = 1;
    send_data.width = 1920;
    send_data.height = 1080;
    send_data.frameRate = 25;
    send_data.type = PT_H265;
    send_data.bitrate = 0;
    send_data.gopsize = 10;
    send_data.transcoding = false;
    send_data.s32SaveFile = true;
    send_data.processor = VIDEO_ENCODER;
    AR_U32 cmdDemo = VCTRL_MSG_START;
    while ((c = getopt_long(argc, argv, ":vBc:C:f:F:n:N:l:t:T:h:w:b:g:i:o:m:M:H:R:", long_options, NULL)) != EOF) {
        switch (c) {
        case 'v':
            send_data.verbose = 1;
            break;
        case 'B':
            //useBind = 1;
            break;
        case 'c':
            send_data.chnId = atoi(optarg);
            break;
        case 'C':
            send_data.transcodingChnId = atoi(optarg);
            break;
        case 'n':
            send_data.picNum = atoi(optarg);
            break;
        case 'N':
            send_data.u32ChnNum = atoi(optarg);
            break;
        case 'l':
            send_data.s32SaveFile = atoi(optarg);
            break;
        case 'f':
            if(cmdDemo == VCTRL_MSG_SET)
                send_data.enUpdate |= UPDATE_FPS;
            send_data.frameRate = atoi(optarg);
            break;
        case 'F':
            send_data.srcframeRate = atoi(optarg);
            break;
        case 't':
            if (strncmp(optarg, "h265", 4) == 0) {
                send_data.type = PT_H265;
            } else if (strncmp(optarg, "h264", 4) == 0) {
                send_data.type = PT_H264;
            } else if (strncmp(optarg, "jpeg", 4) == 0) {
                send_data.type = PT_JPEG;
            } else if (strncmp(optarg, "mjpeg", 5) == 0) {
                send_data.type = PT_MJPEG;
            } else {
                send_data.type = PT_H265;
                printf(stderr, "please identify your codec type %s.\n", optarg);
            }
            break;
        case 'T':
            if (strncmp(optarg, "h265", 4) == 0) {
                send_data.transcodingType = PT_H265;
            } else if (strncmp(optarg, "h264", 4) == 0) {
                send_data.transcodingType = PT_H264;
            } else if (strncmp(optarg, "jpeg", 4) == 0) {
                send_data.transcodingType = PT_JPEG;
            } else if (strncmp(optarg, "mjpeg", 5) == 0) {
                send_data.transcodingType = PT_MJPEG;
            } else {
                send_data.transcodingType = PT_H265;
                printf(stderr, "please identify your codec type %s.\n", optarg);
            }
            break;
        case 'w':
            send_data.width = atoi(optarg);
            break;
        case 'h':
            send_data.height = atoi(optarg);
            break;
        case 'b':
            if(cmdDemo == VCTRL_MSG_SET)
                send_data.enUpdate |= UPDATE_BITRATE;
            send_data.bitrate = atoi(optarg);
            break;
        case 'g':
            if(cmdDemo == VCTRL_MSG_SET)
                send_data.enUpdate |= UPDATE_GOPSIZE;
            send_data.gopsize = atoi(optarg);
            break;
        case 'i':
            /*
            memset(send_data.streamFile, 0, sizeof(send_data.streamFile));
            memcpy(send_data.streamFile, optarg, strlen(optarg));
            */
            memset(inputFile, 0, sizeof(inputFile));
            memcpy(inputFile, optarg, strlen(optarg));
            break;
        case 'o':
            /*
            memset(send_data.yuvFile, 0, sizeof(send_data.yuvFile));
            memcpy(send_data.yuvFile, optarg, strlen(optarg));
            */
            memset(outputFile, 0, sizeof(outputFile));
            memcpy(outputFile, optarg, strlen(optarg));
            break;
        case 'm':
            if (strcmp(optarg, "encoder") == 0)
                send_data.processor = VIDEO_ENCODER;
            else if (strcmp(optarg, "decoder") == 0)
                send_data.processor = VIDEO_DECODER;
            else
                send_data.processor = VIDEO_ENCODER;
            break;
        case 'M':
            send_data.enRcMode = atoi(optarg);
            break;
        case 'R':
            cmdDemo = atoi(optarg);
            break;
        case 0:
            if(cmdDemo == VCTRL_MSG_SET) {
                if (roiIndex) {
                    send_data.roiMap.roiIndex = atoi(optarg);
                    roiIndex = 0;
                    roiUpdateFlag ++;
                } else if (roiIndexEnable) {
                    send_data.roiMap.roiIndexEnable = atoi(optarg);
                    roiIndexEnable = 0;
                    roiUpdateFlag ++;
                } else if (roiQp) {
                    send_data.roiMap.roiQp = atoi(optarg);
                    roiQp = 0;
                    roiUpdateFlag ++;
                } else if (roiStartX) {
                    send_data.roiMap.roiStartX = atoi(optarg);
                    roiStartX = 0;
                    roiUpdateFlag ++;
                } else if (roiStartY) {
                    send_data.roiMap.roiStartY = atoi(optarg);
                    roiStartY = 0;
                    roiUpdateFlag ++;
                } else if (roiWidth) {
                    send_data.roiMap.roiWidth = atoi(optarg);
                    roiWidth = 0;
                    roiUpdateFlag ++;
                } else if (roiHeigth) {
                    send_data.roiMap.roiHeigth = atoi(optarg);
                    roiHeigth = 0;
                    roiUpdateFlag ++;
                } else if (minIQp) {
                    send_data.qpChange.enRcUpate |= UPDATE_MINIQP;
                    send_data.qpChange.minIQp = atoi(optarg);
                    minIQp = 0;
                } else if (maxIQp) {
                    send_data.qpChange.enRcUpate |= UPDATE_MAXIQP;
                    send_data.qpChange.maxIQp = atoi(optarg);
                    maxIQp = 0;
                } else if (minPQp) {
                    send_data.qpChange.enRcUpate |= UPDATE_MINPQP;
                    send_data.qpChange.minPQp = atoi(optarg);
                    minPQp = 0;
                } else if (maxPQp) {
                    send_data.qpChange.enRcUpate |= UPDATE_MAXPQP;
                    send_data.qpChange.maxPQp = atoi(optarg);
                    maxPQp = 0;
                } else if (minBQp) {
                    send_data.qpChange.enRcUpate |= UPDATE_MINBQP;
                    send_data.qpChange.minBQp = atoi(optarg);
                    minBQp = 0;
                } else if (maxBQp) {
                    send_data.qpChange.enRcUpate |= UPDATE_MAXBQP;
                    send_data.qpChange.maxBQp = atoi(optarg);
                    maxBQp = 0;
                } else if (minIprop) {
                    send_data.qpChange.enRcUpate |= UPDATE_MINIPROP;
                    send_data.qpChange.minIprop = atoi(optarg);
                    minIprop = 0;
                } else if (maxIprop) {
                    send_data.qpChange.enRcUpate |= UPDATE_MAXIPROP;
                    send_data.qpChange.maxIprop = atoi(optarg);
                    maxIprop = 0;
                } else if (fixedIQp) {
                    send_data.qpChange.enRcUpate |= UPDATE_FIXEDIQP;
                    send_data.qpChange.fixedIQp = atoi(optarg);
                    fixedIQp = 0;
                } else if (fixedPQp) {
                    send_data.qpChange.enRcUpate |= UPDATE_FIXEDPQP;
                    send_data.qpChange.fixedPQp = atoi(optarg);
                    fixedPQp = 0;
                } else if (fixedBQp) {
                    send_data.qpChange.enRcUpate |= UPDATE_FIXEDBQP;
                    send_data.qpChange.fixedBQp = atoi(optarg);
                    fixedBQp = 0;
                } else if (firstFrameStartQp) {
                    send_data.qpChange.enRcUpate |= UPDATE_FIRSTFRAMESTARTQP;
                    send_data.qpChange.firstFrameStartQp = atoi(optarg);
                    firstFrameStartQp = 0;
                } else if (minQfactor) {
                    send_data.qpChange.enRcUpate |= UPDATE_MINQFACTOR;
                    send_data.qpChange.minQfactor = atoi(optarg);
                    minQfactor = 0;
                } else if (maxQfactor) {
                    send_data.qpChange.enRcUpate |= UPDATE_MAXQFACTOR;
                    send_data.qpChange.maxQfactor = atoi(optarg);
                    maxQfactor = 0;
                }
            }
            if (transCoding) {
                send_data.transcoding = atoi(optarg);
                transCoding = 0;
            }

            if (preLoadPicNum) {
                send_data.preLoadPicNum = atoi(optarg);
                preLoadPicNum = 0;
            }
            break;
        case 'H':
        default:
            /* ignore unknown options */
            usage();
            exit(0);
            break;
        }
    }

    /* non-option has only one, image-file */
    if (argc > optind) {
        usage();
        exit(0);
    }

    if (send_data.processor == VIDEO_ENCODER) {
        memset(send_data.yuvFile,    0, sizeof(send_data.yuvFile));
        memset(send_data.streamFile, 0, sizeof(send_data.streamFile));

        memcpy(send_data.yuvFile,    inputFile,  strlen(inputFile));
        memcpy(send_data.streamFile, outputFile, strlen(outputFile));
    } else if (send_data.processor == VIDEO_DECODER) {
        memset(send_data.streamFile, 0, sizeof(send_data.streamFile));
        memset(send_data.yuvFile,    0, sizeof(send_data.yuvFile));

        memcpy(send_data.streamFile, inputFile, strlen(inputFile));
        memcpy(send_data.yuvFile,    outputFile, strlen(outputFile));
    }

    if (NULL == inputFile) {
        printf("-i input file cannot be empty !\n");
        exit(0);
    }

    if (7 == roiUpdateFlag) {
        send_data.enUpdate |= SET_ROIMAP;
    }

    if ((send_data.qpChange.enRcUpate & UPDATE_MINIQP) || (send_data.qpChange.enRcUpate & UPDATE_MAXIQP) ||
        (send_data.qpChange.enRcUpate & UPDATE_MINPQP) || (send_data.qpChange.enRcUpate & UPDATE_MAXPQP) ||
        (send_data.qpChange.enRcUpate & UPDATE_MINBQP) || (send_data.qpChange.enRcUpate & UPDATE_MAXBQP) ||
        (send_data.qpChange.enRcUpate & UPDATE_MINIPROP) || (send_data.qpChange.enRcUpate & UPDATE_MAXIPROP) ||
        (send_data.qpChange.enRcUpate & UPDATE_FIXEDIQP) || (send_data.qpChange.enRcUpate & UPDATE_FIXEDPQP) ||
        (send_data.qpChange.enRcUpate & UPDATE_FIXEDBQP) || (send_data.qpChange.enRcUpate & UPDATE_FIRSTFRAMESTARTQP) ||
        (send_data.qpChange.enRcUpate & UPDATE_MINQFACTOR) || (send_data.qpChange.enRcUpate & UPDATE_MAXQFACTOR)) {

        send_data.enUpdate |= UPDATE_QP;
        printf("enRcUpdate = 0x%x\n", send_data.qpChange.enRcUpate);

    }

    vctrl_msg msg;
    memset(&msg, 0, sizeof(msg));
    msg.cmd = cmdDemo;
    memcpy(&msg.send_data, &send_data, sizeof(send_data));
    printf("chn%d start, enUpadate = 0x%x\n", send_data.chnId, send_data.enUpdate);
    send_vctrl_msg(&msg);
    printf("chn%d end\n", send_data.chnId);

    return 0;
}
