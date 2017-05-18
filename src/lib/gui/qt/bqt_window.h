#ifndef BQT_WINDOW_H
#define BQT_WINDOW_H

#include <pocas/gui/private/backend.h>
#include <QWidget>

#include "bqt_eventfilter.h"
#include "bqt_control.h"

class QMenuBar;

class Bqt_Window : public Bqt_Control
{
    Q_OBJECT

public:
    Bqt_Window(PG_Window *w, Bqt_Window *parent);
    virtual void setShown(bool shown);
    virtual void setParent(QWidget *parent);
    virtual void setMenu(QMenuBar *menu);
    virtual QWidget *widget();
    virtual ~Bqt_Window();

public slots:
    void close();

signals:
    void closing();

private slots:
    void onWindowEvent(Bqt_EventFilter::FilterArgs *args);

private:
    void updateContainerSize();

    QWidget m_qw;
    QMenuBar *m_mainMenu;
    PG_Window *m_w;
    Bqt_Window *m_parent;
    Bqt_EventFilter m_closeFilter;
    bool m_filterClosing;
    static int m_nWindows;
};

#endif
