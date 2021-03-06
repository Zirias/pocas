#include <pocas/core/event.h>
#include <pocas/core/eventloop.h>

#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QMenuBar>
#include <QPushButton>

#include "bqt_backend.h"
#include "bqt_menu.h"

#include "bqt_window.h"

SOLOCAL int Bqt_Window::m_nWindows = 0;

SOLOCAL Bqt_Window::Bqt_Window(PG_Window *w, Bqt_Window *parent)
    : m_qw(parent ? parent->widget() : 0, parent ? Qt::Dialog : Qt::Window),
      m_mainMenu(0), m_w(w), m_parent(parent), m_filterClosing(true),
      m_wasShown(false)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    m_qw.installEventFilter(&m_closeFilter);
    m_qw.setWindowTitle(QString::fromUtf8(api->window.title(w)));
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
    if (shown)
    {
	if (!m_wasShown)
	{
	    QWidget *pw = m_qw.parentWidget();
	    if (pw)
	    {
		m_qw.move(pw->window()->frameGeometry().topLeft() +
			pw->window()->rect().center() - m_qw.rect().center());
	    }
	    m_wasShown = true;
	}
	m_qw.show();
    }
    else
    {
	m_qw.hide();
    }
}

SOLOCAL void Bqt_Window::setParent(QWidget *parent)
{
    m_qw.setParent(parent);
}

SOLOCAL void Bqt_Window::setMenu(QMenuBar *menu)
{
    delete m_mainMenu;
    m_mainMenu = menu;
    if (menu)
    {
        menu->setParent(&m_qw);
    }
    updateContainerSize();
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
        updateContainerSize();
        break;
    case QEvent::KeyPress:
        QKeyEvent *ke = static_cast<QKeyEvent *>(args->event);
        if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return)
        {
            QWidget *focused = QApplication::focusWidget();
            if (focused)
            {
                QPushButton *focusedButton = dynamic_cast<QPushButton *>(focused);
                if (focusedButton)
                {
                    args->filter = true;
                    focusedButton->click();
                    break;
                }
            }
            for (int i = 0; i < m_qw.children().length(); ++i)
            {
                QPushButton *button = dynamic_cast<QPushButton *>(m_qw.children().at(i));
                if (button)
                {
                    if (button->isDefault())
                    {
                        args->filter = true;
                        button->click();
                    }
                }
            }
        }
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


void Bqt_Window::updateContainerSize()
{
    PG_Bounds wb = { 0, 0, m_qw.width(), m_qw.height() };
    if (m_mainMenu)
    {
        QSize hintedMenuSize = m_mainMenu->sizeHint();
        int menuHeight = hintedMenuSize.height();
        wb.topLeft.y += menuHeight;
        if (menuHeight > wb.size.height)
        {
            wb.size.height = 0;
        }
        else
        {
            wb.size.height -= menuHeight;
        }
        m_mainMenu->resize(m_qw.width(), menuHeight);
    }
    PG_qtBackend->privateApi->container.setBounds(m_w, &wb);
}

SOLOCAL Bqt_Window::~Bqt_Window()
{
    delete m_mainMenu;
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

SOLOCAL_CDECL void Bqt_Window_setMenu(PG_Window *w, PG_Menu *menu)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    Bqt_Window *bw = (Bqt_Window *)api->backendObject(w);
    Bqt_Menu *bm = (Bqt_Menu *)api->backendObject(menu);
    bw->setMenu(bm->createMenuBar());
}

SOLOCAL_CDECL void Bqt_Window_close(PG_Window *w)
{
    Bqt_Window *bw = (Bqt_Window *)PG_qtBackend->privateApi->backendObject(w);
    if (bw) bw->close();
}

SOLOCAL_CDECL void Bqt_Window_showModal(PG_Window *w)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    Bqt_Window *bw = (Bqt_Window *)api->backendObject(w);
    if (!bw) return;
    bw->widget()->setWindowModality(Qt::ApplicationModal);
    api->control.setShown(w, 1);
}

SOLOCAL_CDECL void Bqt_Window_hideModal(PG_Window *w)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    api->control.setShown(w, 0);
    Bqt_Window *bw = (Bqt_Window *)api->backendObject(w);
    if (!bw) return;
    bw->widget()->setWindowModality(Qt::NonModal);
}

SOLOCAL_CDECL void Bqt_Window_destroy(PG_Window *w)
{
    Bqt_Window *bw = (Bqt_Window *)PG_qtBackend->privateApi->backendObject(w);
    delete bw;
}
