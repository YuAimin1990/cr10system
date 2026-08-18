
#ifndef __UTILS_NETIO_H__
#define __UTILS_NETIO_H__

#include <ctype.h>

int fd_wait_readable (int fd, int milliseconds);
int fd_wait_writeable(int fd, int milliseconds);

ssize_t readn_timedwait (int fd, void *buf, size_t count);
ssize_t writen_timedwait(int fd, void *buf, size_t count);

int socket_set_nonblock(int socket, int enable);

int tcp_server (char *ipaddr, int port);
int tcp_connect(char *ipaddr, int port);

int udp_server (char *ipaddr, int port);
int udp_connect(char *ipaddr, int port);

#endif /* __UTILS_NETIO_H__ */
