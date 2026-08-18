#ifndef __RECORD_BITS_H__
#define __RECORD_BITS_H__

#include <stdint.h>

typedef struct JBits JBits;

JBits *j_bits_alloc(uint32_t length);
int j_bits_free(JBits *jb);

int j_bits_set(JBits *jb, uint32_t n);
int j_bits_clr(JBits *jb, uint32_t n);

int j_bits_get(JBits *jb, uint32_t n);

int j_bits_reset(JBits *jb);

#endif /* __RECORD_BITS_H__ */
