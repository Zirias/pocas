#ifndef BQT_LABEL_H
#define BQT_LABEL_H

#include <pocas/gui/private/backend.h>
#include <QLabel>

#include "bqt_control.h"

class Bqt_Label : public Bqt_Control
{
    Q_OBJECT

public:
    Bqt_Label(PG_Label *l, const QString& text);
    virtual void setParent(QWidget *parent);
    virtual QWidget *widget();

public slots:
    void setText(const QString& text);

private:
    QLabel m_ql;
    PG_Label *m_l;
};

#endif
