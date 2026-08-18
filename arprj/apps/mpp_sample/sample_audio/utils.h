#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

// A simple logging macro
#define LOG_INFO(fmt, ...)  fprintf(stdout, "[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) fprintf(stderr, "\033[0;31m[ERROR] %s:%d: " fmt "\033[0;39m\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  fprintf(stdout, "\033[0;33m[WARN] " fmt "\033[0;39m\n", ##__VA_ARGS__)


// A macro to check return values, similar to the original
#define CHECK_RET(expression, name) \
    do { \
        int ret_val = (expression); \
        if (0 != ret_val) { \
            LOG_ERROR("%s failed with code %#x!", name, ret_val); \
            return ret_val; \
        } \
    } while(0)

#endif // UTILS_H