#ifndef BQT_EVENTLOOP_H
#define BQT_EVENTLOOP_H

#include <QObject>
#include <QBasicTimer>

class QApplication;

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
    QBasicTimer m_timer;
    QApplication *m_app;
};

#endif
