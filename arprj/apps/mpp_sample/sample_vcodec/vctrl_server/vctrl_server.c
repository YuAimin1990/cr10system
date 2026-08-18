#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <errno.h>
#include <stdarg.h>

#include "vctrl_handler.h"
#include "signal_handler.h"
#include "vctrl_list.h"

static struct option long_options[] = {
    {"coreClock", no_argument, NULL, 'C'},
    {"bpuClock", no_argument, NULL, 'B'},
    {"jpuClock", required_argument, NULL, 'J'},
    {NULL, 0, NULL, 0}
};

int main(int argc, char **argv)
{
    int c;
    int fd_count = 0;
    struct pollfd ufds[2];
    int signal_fd_init = 0;
    int vctrl_set_fd_init = 0;
    int s32Ret = AR_FAILURE;
    unsigned int coreClock = 0;
    unsigned int bpuClock = 0;
    unsigned int jpuClock = 0;

    ar_hal_log_init();
    ar_log_shm_set_enabled(0);
    signal_init();
    start_vctrl_service();
    s32Ret = AR_MPI_SYS_Init();

    if (0 != s32Ret) {
        printf("AR_MPI_SYS_Init failed!\n");
        goto error;
    }

    while ((c = getopt_long(argc, argv, ":B:C:J:", long_options, NULL)) != EOF) {
        switch (c) {
        case 'C':
            coreClock = atoi(optarg);
            break;
        case 'B':
            bpuClock = atoi(optarg);
            break;
        case 'J':
            jpuClock = atoi(optarg);
            break;
        default:
            break;
       }
    }

    for(;;) {
        int nr, i, timeout = -1;

        if (!vctrl_set_fd_init && get_vctrl_set_fd() > 0) {
            ufds[fd_count].fd = get_vctrl_set_fd();
            ufds[fd_count].events = POLLIN;
            ufds[fd_count].revents = 0;
            fd_count++;
            vctrl_set_fd_init = 1;
        }

        if (!signal_fd_init && get_signal_fd() > 0) {
            ufds[fd_count].fd = get_signal_fd();
            ufds[fd_count].events = POLLIN;
            ufds[fd_count].revents = 0;
            fd_count++;
            signal_fd_init = 1;
        }

        nr = poll(ufds, fd_count, timeout);
        if (nr <= 0)
            continue;

        for (i = 0; i < fd_count; i++) {
            if (ufds[i].revents & POLLIN) {
                if (ufds[i].fd == get_vctrl_set_fd())
                    handle_vctrl_set_fd(coreClock, bpuClock, jpuClock);
                else if (ufds[i].fd == get_signal_fd())
                    handle_signal();
            }
        }
    }

error:
    ar_hal_log_close();
    AR_MPI_SYS_Exit();

}
