#include <stdlib.h>

#include <pocas/core/list.h>
#include <pocas/core/string.h>

#include "backend_internal.h"

#include <pocas/gui/menu.h>

struct Menu
{
    B_Menu *b;
    List *items;
};

struct MenuItem
{
    B_MenuItem *b;
    char *text;
    Command *command;
    Menu *subMenu;
};

static void deleteItem(void *item)
{
    MenuItem_destroy(item);
}

SOEXPORT Menu *Menu_create()
{
    Menu *self = malloc(sizeof(Menu));
    self->items = List_create(0, deleteItem, 0);

    const Backend *b = Backend_current();
    self->b = b->Menu_create ? b->Menu_create(self) : 0;
    return self;
}

SOEXPORT ListIterator *Menu_items(const Menu *self)
{
    return List_iterator(self->items);
}

SOEXPORT void Menu_addItem(Menu *self, MenuItem *item)
{
    List_append(self->items, item);

    const Backend *b = Backend_current();
    if (b->Menu_addItem) b->Menu_addItem(self->b, ((Frontend *)item)->b);
}

SOEXPORT void Menu_removeItem(Menu *self, MenuItem *item)
{
    List_remove(self->items, item);

    const Backend *b = Backend_current();
    if (b->Menu_removeItem) b->Menu_removeItem(self->b, ((Frontend *)item)->b);
}

SOEXPORT void Menu_destroy(Menu *self)
{
    if (!self) return;
    List_destroy(self->items);
    const Backend *b = Backend_current();
    if (b->Menu_destroy) b->Menu_destroy(self->b);
    free(self);
}

SOEXPORT MenuItem *MenuItem_create(const char *text)
{
    MenuItem *self = malloc(sizeof(MenuItem));
    self->text = String_copy(text);
    self->command = 0;
    self->subMenu = 0;
    const Backend *b = Backend_current();
    self->b = b->MenuItem_create ? b->MenuItem_create(self) : 0;
    return self;
}

SOEXPORT const char *MenuItem_text(const MenuItem *self)
{
    return self->text;
}

SOEXPORT void MenuItem_setText(MenuItem *self, const char *text)
{
    free(self->text);
    self->text = String_copy(text);
}

SOEXPORT Command *MenuItem_command(const MenuItem *self)
{
    return self->command;
}

SOEXPORT void MenuItem_setCommand(MenuItem *self, Command *command)
{
    self->command = command;
}

SOEXPORT Menu *MenuItem_subMenu(const MenuItem *self)
{
    return self->subMenu;
}

SOEXPORT void MenuItem_setSubMenu(MenuItem *self, Menu *subMenu)
{
    self->subMenu = subMenu;
}

SOEXPORT void MenuItem_destroy(MenuItem *self)
{
    if (!self) return;
    free(self->text);
    Menu_destroy(self->subMenu);
    const Backend *b = Backend_current();
    if (b->MenuItem_destroy) b->MenuItem_destroy(self->b);
    free(self);
}
