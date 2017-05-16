#ifndef BQT_EVENTLOOP_H
#define BQT_EVENTLOOP_H

#include <QEventLoop>
#include <QBasicTimer>

#include "decl.h"

class Bqt_EventLoop : QObject
{
    Q_OBJECT

public:
    Bqt_EventLoop() {}
    int processEvents(int timeout);

protected:
    void timerEvent(QTimerEvent *event);

private:
    bool timedOut;
    QEventLoop loop;
    QBasicTimer timer;
};

#endif
