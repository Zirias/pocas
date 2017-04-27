#ifndef BACKEND_INTERNAL_H
#define BACKEND_INTERNAL_H

#include <pocas/gui/messagebox.h>

#include <pocas/gui/backend.h>

typedef struct Window Window;
typedef struct B_Window B_Window;
typedef struct Menu Menu;
typedef struct B_Menu B_Menu;
typedef struct MenuItem MenuItem;
typedef struct B_MenuItem B_MenuItem;
typedef struct Command Command;
typedef struct B_Command B_Command;

typedef struct Frontend
{
    void *b;
} Frontend;

struct Backend
{
    const char *(*name)(void);

    B_Window *(*Window_create)(Window *w);
    void (*Window_show)(B_Window *self);
    void (*Window_hide)(B_Window *self);
    void (*Window_setMenu)(B_Window *self, B_Menu *menu);
    void (*Window_close)(B_Window *self);
    void (*Window_destroy)(B_Window *self);

    B_Menu *(*Menu_create)(Menu *m);
    void (*Menu_addItem)(B_Menu *self, B_MenuItem *item);
    void (*Menu_removeItem)(B_Menu *self, B_MenuItem *item);
    void (*Menu_destroy)(B_Menu *self);

    B_MenuItem *(*MenuItem_create)(MenuItem *m);
    void (*MenuItem_destroy)(B_MenuItem *self);

    B_Command *(*Command_create)(Command *c);
    void (*Command_destroy)(B_Command *self);

    MessageBoxButton (*MessageBox_show)(const Window *w, const char *title,
            const char *text, MessageBoxButton buttons, MessageBoxStyle style);
};

#endif // BACKEND_INTERNAL_H
