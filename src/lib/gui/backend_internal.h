#ifndef BACKEND_INTERNAL_H
#define BACKEND_INTERNAL_H

#include <pocas/gui/backend.h>

typedef struct Window Window;
typedef struct B_Window B_Window;

struct Backend
{
    const char *(*name)(void);

    B_Window *(*Window_create)(Window *w);
    void (*Window_show)(B_Window *self);
    void (*Window_hide)(B_Window *self);
    void (*Window_destroy)(B_Window *self);

};

#endif // BACKEND_INTERNAL_H
