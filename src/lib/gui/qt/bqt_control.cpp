#include "bqt_control.h"
#include "bqt_backend.h"

#include <QWidget>

SOLOCAL Bqt_Control::Bqt_Control()
{
    Bqt_Backend_init();
}

SOLOCAL void Bqt_Control::setShown(bool shown)
{
    QWidget *w = widget();
    if (!w) return;
    if (shown)
    {
        if (w->parent())
        {
            w->show();
        }
    }
    else
    {
        w->hide();
    }
}

SOLOCAL void Bqt_Control::setEnabled(bool enabled)
{
    QWidget *w = widget();
    if (!w) return;
    w->setEnabled(enabled);
}

SOLOCAL void Bqt_Control::setGeometry(const QRect &rect)
{
    QWidget *w = widget();
    if (!w) return;
    w->setGeometry(rect);
}

SOLOCAL void Bqt_Control::focus()
{
    QWidget *w = widget();
    if (!w) return;
    w->setFocus();
}

SOLOCAL_CDECL void Bqt_Control_setShown(void *control, int shown)
{
    Bqt_Control *bc = (Bqt_Control *)
            PG_qtBackend->privateApi->backendObject(control);
    if (bc) bc->setShown(shown);
}

SOLOCAL_CDECL void Bqt_Control_setEnabled(void *control, int enabled)
{
    Bqt_Control *bc = (Bqt_Control *)
            PG_qtBackend->privateApi->backendObject(control);
    if (bc) bc->setEnabled(enabled);
}

SOLOCAL_CDECL void Bqt_Control_setContainer(void *control, void *container)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    Bqt_Control *c = (Bqt_Control *)api->backendObject(control);
    if (!c) return;
    if (!container)
    {
        c->setParent(0);
    }
    else
    {
        Bqt_Control *cbc = (Bqt_Control *)api->backendObject(container);
        while (!cbc || !cbc->widget())
        {
            void *cc = api->controlObject(container);
            if (!cc) return;
            container = api->control.container(container);
            if (!container) return;
            cbc = (Bqt_Control *)api->backendObject(container);
        }
        c->setParent(cbc->widget());
    }
}

SOLOCAL_CDECL void Bqt_Control_setBounds(void *control, const PG_Bounds *b)
{
    Bqt_Control *c = (Bqt_Control *)PG_qtBackend->privateApi->backendObject(control);
    if (c)
    {
        c->setGeometry(QRect(b->topLeft.x, b->topLeft.y,
                b->size.width, b->size.height));
    }
}

SOLOCAL_CDECL void Bqt_Control_focus(void *control)
{
    Bqt_Control *c = (Bqt_Control *)PG_qtBackend->privateApi->backendObject(control);
    if (c) c->focus();
}
