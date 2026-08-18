#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include "vctrl_common.h"

static int signal_fd = -1;
static int signal_recv_fd = -1;

static void sigchld_handler(int s)
{
    write(signal_fd, &s, 1);
}


void handle_signal(void)
{
    char tmp[32];
    read(signal_recv_fd, tmp, sizeof(tmp));
    //todo
}

void signal_init(void)
{
    int s[2];

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sigchld_handler;
    act.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGINT, &act, 0);

    /* create a signalling mechanism for the sigchld handler */
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, s) == 0) {
        signal_fd = s[0];
        signal_recv_fd = s[1];
        fcntl(s[0], F_SETFD, FD_CLOEXEC);
        fcntl(s[0], F_SETFL, O_NONBLOCK);
        fcntl(s[1], F_SETFD, FD_CLOEXEC);
        fcntl(s[1], F_SETFL, O_NONBLOCK);
    }

    handle_signal();
}

int get_signal_fd()
{
    return signal_recv_fd;
}

