
#include "vo.h"

static vo_t* vo = NULL;

extern vo_t vo_android;

static void vo_register_one(vo_t* o) {
    if (!vo) {
        vo = o;
        vo->next = NULL;
    }
    else {
        vo_t* temp = vo;

        while (temp->next) {
            if (!strcmp(o->name, temp->name))
                return;
            temp = temp->next;
        }
        temp->next = o;
        temp->next->next = NULL;
    }
}

void vo_register_all() {
    vo_register_one(&vo_android);
}

vo_t* vo_find_by_name(const char* name) {
    vo_t* result;

    result = vo;
    while(result) {
        if (!strcmp(result->name, name))
            break;
        result = result->next;
    }

    return result;
}

