#ifndef GUI_INTERNAL_H
#define GUI_INTERNAL_H

typedef struct Gui Gui;

Gui *Gui_create(void);
int Gui_run(Gui *self);
void Gui_dispose(Gui *self);
void Gui_destroy(Gui *self);

#endif
