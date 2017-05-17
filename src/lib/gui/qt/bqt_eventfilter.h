#ifndef BQT_EVENTFILTER_H
#define BQT_EVENTFILTER_H

#include <QObject>

#include <pocas/gui/backend/qt.h>

class QEvent;

class Bqt_EventFilter : public QObject
{
    Q_OBJECT

public:
    struct FilterArgs
    {
        QEvent *event;
        bool filter;
    };

    Bqt_EventFilter();

signals:
    eventOccured(Bqt_EventFilter::FilterArgs *args);

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
};

#endif
