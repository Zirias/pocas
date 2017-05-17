#include <pocas/core/event.h>
#include <pocas/core/eventloop.h>
#include <pocas/gui/bounds.h>

#include <QEvent>

#include "bqt_window.h"
#include "bqt_backend.h"

SOLOCAL int Bqt_Window::m_nWindows = 0;

SOLOCAL Bqt_Window::Bqt_Window(PG_Window *w, Bqt_Window *parent)
    : m_qw(parent ? parent->widget() : 0, parent ? Qt::Dialog : Qt::Window),
      m_w(w), m_parent(parent), m_filterClosing(true)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    m_qw.installEventFilter(&m_closeFilter);
    m_qw.setWindowTitle(QString(api->window.title(w)));
    m_qw.resize(api->window.width(w), api->window.height(w));
    connect(&m_closeFilter, SIGNAL(eventOccured(Bqt_EventFilter::FilterArgs*)),
            this, SLOT(onWindowEvent(Bqt_EventFilter::FilterArgs*)));
    if (m_parent)
    {
        connect(m_parent, SIGNAL(closing()), this, SLOT(close()));
    }
    ++m_nWindows;
}

SOLOCAL void Bqt_Window::setShown(bool shown)
{
    if (shown) m_qw.show();
    else m_qw.hide();
}

SOLOCAL void Bqt_Window::setParent(QWidget *parent)
{
    m_qw.setParent(parent);
}

SOLOCAL QWidget *Bqt_Window::widget()
{
    return &m_qw;
}

SOLOCAL void Bqt_Window::onWindowEvent(Bqt_EventFilter::FilterArgs *args)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
    switch (args->event->type())
    {
    case QEvent::Close:
        if (!m_filterClosing) break;
        args->filter = true;
        api->window.close(m_w);
        break;
    case QEvent::Resize:
        PG_Bounds wb = { 0, 0, (unsigned)m_qw.width(), (unsigned )m_qw.height() };
        api->container.setBounds(m_w, &wb);
        break;
    }
#pragma GCC diagnostic pop
}

SOLOCAL void Bqt_Window::close()
{
    emit closing();
    if (m_parent)
    {
        disconnect(m_parent, SIGNAL(closing()), this, SLOT(close()));
    }
    m_filterClosing = false;
    m_qw.close();
    m_filterClosing = true;
    if (!--m_nWindows)
    {
        PC_Event *lwc = PG_qtBackend->privateApi->window.lastWindowClosedEvent();
        PC_EventArgs lwcArgs = PC_EventArgs_init(lwc, 0, 0);
        PC_Event_raise(lwc, &lwcArgs);
        if (!lwcArgs.handled)
        {
            PC_EventLoop_exit(0);
        }
    }
}

SOLOCAL_CDECL int Bqt_Window_create(PG_Window *w)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    PG_Window *parent = api->window.parent(w);
    Bqt_Window *pbw = 0;
    if (parent)
    {
        pbw = (Bqt_Window *)api->backendObject(parent);
    }
    Bqt_Window *bw = new Bqt_Window(w, pbw);
    api->setBackendObject(w, bw);
    return 1;
}

SOLOCAL_CDECL void Bqt_Window_close(PG_Window *w)
{
    Bqt_Window *bw = (Bqt_Window *)PG_qtBackend->privateApi->backendObject(w);
    if (bw) bw->close();
}

SOLOCAL_CDECL void Bqt_Window_destroy(PG_Window *w)
{
    Bqt_Window *bw = (Bqt_Window *)PG_qtBackend->privateApi->backendObject(w);
    delete bw;
}
