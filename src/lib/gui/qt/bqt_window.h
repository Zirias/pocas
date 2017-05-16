#ifndef BQT_WINDOW_H
#define BQT_WINDOW_H

#include <pocas/gui/private/backend.h>
#include <QWindow>

class Bqt_Window : QWindow
{
    Q_OBJECT

public:
    Bqt_Window(PG_Window *fw);

private:
    PG_Window *w;
};

#endif
