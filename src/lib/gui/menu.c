#include <stdlib.h>

#include <pocas/core/event.h>
#include <pocas/core/list.h>
#include <pocas/core/string.h>

#include "internal.h"

#include <pocas/gui/command.h>
#include <pocas/gui/menu.h>

struct Menu
{
    GuiClass gc;
    List *items;
};

struct MenuItem
{
    GuiClass gc;
    char *text;
    Command *command;
    Event *selected;
    Menu *subMenu;
};

static void deleteItem(void *item)
{
    MenuItem_destroy(item);
}

SOEXPORT Menu *Menu_create()
{
    Menu *self = malloc(sizeof(Menu));
    GCINIT(self);
    self->items = List_create(0, deleteItem, 0);

    const Backend *b = Backend_current();
    if (b->backendApi.menu.create) b->backendApi.menu.create(self);
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
    if (b->backendApi.menu.addItem) b->backendApi.menu.addItem(self, item);
}

SOEXPORT void Menu_removeItem(Menu *self, MenuItem *item)
{
    List_remove(self->items, item);

    const Backend *b = Backend_current();
    if (b->backendApi.menu.removeItem) b->backendApi.menu.removeItem(self, item);
}

SOEXPORT void Menu_destroy(Menu *self)
{
    if (!self) return;
    List_destroy(self->items);
    const Backend *b = Backend_current();
    if (b->backendApi.menu.destroy) b->backendApi.menu.destroy(self);
    free(self);
}

SOEXPORT MenuItem *MenuItem_create(const char *text)
{
    MenuItem *self = malloc(sizeof(MenuItem));
    GCINIT(self);
    self->text = String_copy(text);
    self->command = 0;
    self->subMenu = 0;
    self->selected = Event_create("selected");
    const Backend *b = Backend_current();
    if (b->backendApi.menuItem.create) b->backendApi.menuItem.create(self);
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

SOEXPORT void MenuItem_select(MenuItem *self)
{
    EventArgs args = EventArgs_init(self->selected, self, 0);
    Event_raise(self->selected, &args);
    if (!args.handled && self->command)
    {
        Command_invoke(self->command);
    }
}

SOEXPORT Event * MenuItem_selectedEvent(const MenuItem *self)
{
    return self->selected;
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
    Event_destroy(self->selected);
    const Backend *b = Backend_current();
    if (b->backendApi.menuItem.destroy) b->backendApi.menuItem.destroy(self);
    free(self);
}
