
#ifndef VO_H
#define VO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "queue.h"

typedef struct _vo_ {
    char* name;
    int (*init)();
    void (*free)();
    int (*display)(Picture*, void* extra);
    struct _vo_* next;
}vo_t;

void vo_register_all();

vo_t* vo_find_by_id(int id);
vo_t* vo_find_by_name(const char* name);

#ifdef __cplusplus
}
#endif


#endif

