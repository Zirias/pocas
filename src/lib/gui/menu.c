#include <stdlib.h>

#include <pocas/core/event.h>
#include <pocas/core/list.h>
#include <pocas/core/string.h>

#include <pocas/gui/backend.h>
#include <pocas/gui/command.h>
#include <pocas/gui/menu.h>

#include "internal.h"

struct PG_Menu
{
    GuiClass gc;
    PC_List *items;
};

struct PG_MenuItem
{
    GuiClass gc;
    char *text;
    PG_Command *command;
    PC_Event *selected;
    PG_Menu *subPG_Menu;
};

static void deleteItem(void *item)
{
    PG_MenuItem_destroy(item);
}

SOEXPORT PG_Menu *PG_Menu_create()
{
    PG_Menu *self = malloc(sizeof(PG_Menu));
    GCINIT(self);
    self->items = PC_List_create(0, deleteItem, 0);

    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.menu.create) b->backendApi.menu.create(self);
    return self;
}

SOEXPORT PC_ListIterator *PG_Menu_items(const PG_Menu *self)
{
    return PC_List_iterator(self->items);
}

SOEXPORT void PG_Menu_addItem(PG_Menu *self, PG_MenuItem *item)
{
    PC_List_append(self->items, item);

    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.menu.addItem) b->backendApi.menu.addItem(self, item);
}

SOEXPORT void PG_Menu_removeItem(PG_Menu *self, PG_MenuItem *item)
{
    PC_List_remove(self->items, item);

    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.menu.removeItem) b->backendApi.menu.removeItem(self, item);
}

SOEXPORT void PG_Menu_destroy(PG_Menu *self)
{
    if (!self) return;
    PC_List_destroy(self->items);
    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.menu.destroy) b->backendApi.menu.destroy(self);
    free(self);
}

SOEXPORT PG_MenuItem *PG_MenuItem_create(const char *text)
{
    PG_MenuItem *self = malloc(sizeof(PG_MenuItem));
    GCINIT(self);
    self->text = PC_String_copy(text);
    self->command = 0;
    self->subPG_Menu = 0;
    self->selected = PC_Event_create("selected");
    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.menuItem.create) b->backendApi.menuItem.create(self);
    return self;
}

SOEXPORT const char *PG_MenuItem_text(const PG_MenuItem *self)
{
    return self->text;
}

SOEXPORT void PG_MenuItem_setText(PG_MenuItem *self, const char *text)
{
    free(self->text);
    self->text = PC_String_copy(text);
}

SOEXPORT PG_Command *PG_MenuItem_command(const PG_MenuItem *self)
{
    return self->command;
}

SOEXPORT void PG_MenuItem_setCommand(PG_MenuItem *self, PG_Command *command)
{
    self->command = command;
}

SOEXPORT void PG_MenuItem_select(PG_MenuItem *self)
{
    PC_EventArgs args = PC_EventArgs_init(self->selected, self, 0);
    PC_Event_raise(self->selected, &args);
    if (!args.handled && self->command)
    {
        PG_Command_invoke(self->command);
    }
}

SOEXPORT PC_Event * PG_MenuItem_selectedEvent(const PG_MenuItem *self)
{
    return self->selected;
}

SOEXPORT PG_Menu *PG_MenuItem_subMenu(const PG_MenuItem *self)
{
    return self->subPG_Menu;
}

SOEXPORT void PG_MenuItem_setSubMenu(PG_MenuItem *self, PG_Menu *subPG_Menu)
{
    self->subPG_Menu = subPG_Menu;
}

SOEXPORT void PG_MenuItem_destroy(PG_MenuItem *self)
{
    if (!self) return;
    free(self->text);
    PG_Menu_destroy(self->subPG_Menu);
    PC_Event_destroy(self->selected);
    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.menuItem.destroy) b->backendApi.menuItem.destroy(self);
    free(self);
}
