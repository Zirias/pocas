#include <pocas/core/eventloop.h>

#include "bqt_eventloop.h"

static Bqt_EventLoop eventLoop;

SOLOCAL int Bqt_EventLoop::processEvents(int timeout)
{
    if (timeout < 0)
    {
        return (int) loop.processEvents(
                    QEventLoop::ProcessEventsFlag::WaitForMoreEvents);
    }
    else if (!timeout)
    {
        return (int) loop.processEvents();
    }
    else
    {
        timedOut = false;
        timer.start(timeout, this);
        loop.processEvents(
                    QEventLoop::ProcessEventsFlag::WaitForMoreEvents);
        timer.stop();
        return timedOut ? 0 : 1;
    }
}

SOLOCAL void Bqt_EventLoop::timerEvent(QTimerEvent *event)
{
    (void)event;
    timedOut = true;
}

static int processEvents(int timeout)
{
    return eventLoop.processEvents(timeout);
}

SOLOCAL_CDECL void installEventLoop()
{
    PC_EventLoop_replaceEventProcessor(&processEvents);
}
