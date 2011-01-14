
#ifndef UTILITY_H
#define UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

void debug(const char* fmt, ...);
void dump(const void* addr, int size);

#ifdef __cplusplus
}
#endif

#endif

