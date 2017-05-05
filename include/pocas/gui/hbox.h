#ifndef POCAS_GUI_HBOX_H
#define POCAS_GUI_HBOX_H

#include <pocas/gui/decl.h>

typedef struct HBox HBox;

typedef struct ListIterator ListIterator;

DECLEXPORT HBox *HBox_create(void);

DECLEXPORT ListIterator *HBox_controls(const HBox *self);
DECLEXPORT void HBox_addControl(HBox *self, void *control);
DECLEXPORT void HBox_removeControl(HBox *self, void *control);

DECLEXPORT void HBox_destroy(HBox *self);

#endif
