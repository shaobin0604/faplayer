
#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void debug(const char* fmt, ...);
void dump(const void* addr, int size);
int64_t gettime();

#ifdef __cplusplus
}
#endif

#endif

