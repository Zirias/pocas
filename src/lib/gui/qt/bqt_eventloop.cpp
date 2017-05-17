#include <pocas/core/eventloop.h>

#include <QAbstractEventDispatcher>
#include "bqt_eventloop.h"

static Bqt_EventLoop eventLoop;

static int argc = 1;
static const char *argv[] = { "pocasgui_qt" };

SOLOCAL Bqt_EventLoop::Bqt_EventLoop()
        : m_app(argc, (char **)argv, 0)
{}

SOLOCAL int Bqt_EventLoop::processEvents(int timeout)
{
    if (timeout < 0)
    {
        return (int) m_app.eventDispatcher()->processEvents(
                    QEventLoop::ProcessEventsFlag::WaitForMoreEvents);
    }
    else if (!timeout)
    {
        return (int) m_app.eventDispatcher()->processEvents(0);
    }
    else
    {
        m_timedOut = false;
        m_timer.start(timeout, this);
        m_app.eventDispatcher()->processEvents(
                    QEventLoop::ProcessEventsFlag::WaitForMoreEvents);
        m_timer.stop();
        return m_timedOut ? 0 : 1;
    }
}

SOLOCAL void Bqt_EventLoop::timerEvent(QTimerEvent *event)
{
    (void)event;
    m_timedOut = true;
}

static int processEvents(int timeout)
{
    return eventLoop.processEvents(timeout);
}

SOLOCAL_CDECL PC_EventProcessor Bqt_EventLoop_install()
{
    return PC_EventLoop_replaceEventProcessor(&processEvents);
}
