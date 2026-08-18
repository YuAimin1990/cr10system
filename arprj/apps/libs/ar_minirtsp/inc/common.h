#include <stdio.h>
#include <string.h>
#include <limits.h>

#ifndef FF_ARRAY_ELEMS
#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))
#endif
