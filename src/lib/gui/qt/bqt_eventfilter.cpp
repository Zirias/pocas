#include "bqt_eventfilter.h"

SOLOCAL Bqt_EventFilter::Bqt_EventFilter()
{}

SOLOCAL bool Bqt_EventFilter::eventFilter(QObject *obj, QEvent *event)
{
    FilterArgs args = { event, false };
    emit eventOccured(&args);
    if (args.filter) return true;
    return QObject::eventFilter(obj, event);
}
