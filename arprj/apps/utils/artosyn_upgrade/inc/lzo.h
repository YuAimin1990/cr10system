#ifndef __LZO_H__
#define __LZO_H__

int lzo1x_decompress_safe(const unsigned char *in, size_t in_len,
			unsigned char *out, size_t *out_len);

#endif
