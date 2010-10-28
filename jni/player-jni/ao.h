
#ifndef AO_H
#define AO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "queue.h"

typedef struct _ao_ {
    char* name;
    int (*init)();
    void (*free)();
    int (*play)(Samples*, void* extra);
    struct _ao_* next;
}ao_t;

void ao_register_all();

ao_t* ao_find_by_name(const char* name);

#ifdef __cplusplus
}
#endif


#endif

