#ifndef BQT_BUTTON_H
#define BQT_BUTTON_H

#include <pocas/gui/private/backend.h>
#include <QPushButton>

#include "bqt_control.h"

class Bqt_Button : public Bqt_Control
{
    Q_OBJECT

public:
    Bqt_Button(PG_Button *b, const QString& text);
    virtual void setParent(QWidget *parent);
    virtual QWidget *widget();

public slots:
    void setText(const QString& text);

private slots:
    void onClick(bool checked);

private:
    QPushButton m_qb;
    PG_Button *m_b;
};

#endif
