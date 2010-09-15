
#include "ao.h"

static ao_t* ao = 0;

extern ao_t ao_android;

static void ao_register_one(ao_t* o) {
    if (!ao) {
        ao = o;
        ao->next = 0;
    }
    else {
        ao_t* temp = ao;

        while (temp->next) {
            if (!strcmp(o->name, temp->name))
                return;
            temp = temp->next;
        }
        temp->next = o;
        temp->next->next = 0;
    }
}

void ao_register_all() {
    ao_register_one(&ao_android);
}

ao_t* ao_find_by_name(const char* name) {
    ao_t* result;

    result = ao;
    while(result) {
        if (!strcmp(result->name, name))
            break;
        result = result->next;
    }

    return result;
}

