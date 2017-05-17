#ifndef BQT_EVENTLOOP_H
#define BQT_EVENTLOOP_H

#include <QApplication>
#include <QBasicTimer>

#include <pocas/gui/backend/qt.h>

class Bqt_EventLoop : public QObject
{
    Q_OBJECT

public:
    Bqt_EventLoop();
    int processEvents(int timeout);

protected:
    void timerEvent(QTimerEvent *event);

private:
    bool m_timedOut;
    QApplication m_app;
    QBasicTimer m_timer;
};

#endif
