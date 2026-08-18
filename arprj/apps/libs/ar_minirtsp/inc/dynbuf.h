#ifndef __UTILS_DYNBUF_H__
#define __UTILS_DYNBUF_H__

typedef struct JDynBuf {
	int bufsiz;

	char *buffer;
	char *bufptr;
	char *bufend;

	int write_flag;
} JDynBuf;

int j_dynbuf_read(JDynBuf *pb, int (*cb)(char *buf, int size, void *ctx), void *ctx);

JDynBuf *j_dynbuf_alloc(int packet_size);
int j_dynbuf_free(JDynBuf *pb);

int j_dynbuf_size(JDynBuf *pb);
int j_dynbuf_space(JDynBuf *pb);

void j_dynbuf_drop(JDynBuf *pb);
void j_dynbuf_read_prepare(JDynBuf *pb);
int j_dynbuf_copy(JDynBuf *dst, JDynBuf *src, int size, int auto_expand);
int j_dynbuf_move(JDynBuf *dst, JDynBuf *src, int size, int auto_expand);
void *j_dynbuf_get(JDynBuf *pb);
int j_dynbuf_drop_front(JDynBuf *pb, int size);
int j_dynbuf_drop_tail(JDynBuf *pb, int size);
int j_dynbuf_expand(JDynBuf *pb, int more);
int j_dynbuf_append(JDynBuf *pb, char *buf, int len);
int j_dynbuf_printf(JDynBuf *pb, const char *fmt, ...);

#define AVIO_FLAG_WRITE	(1)
#define AVIO_FLAG_READ	(2)

#endif
