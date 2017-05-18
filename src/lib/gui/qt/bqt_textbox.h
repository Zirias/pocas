#ifndef BQT_TEXTBOX_H
#define BQT_TEXTBOX_H

#include <pocas/gui/private/backend.h>
#include <QLineEdit>

#include "bqt_control.h"

class Bqt_TextBox : public Bqt_Control
{
    Q_OBJECT

public:
    Bqt_TextBox(PG_TextBox *tb, PG_TextBox_textChanged changed);
    virtual void setParent(QWidget *parent);
    virtual QWidget *widget();
    virtual void focus();

public slots:
    void setText(const QString& text);

private slots:
    void textChanged(const QString& text);

private:
    QLineEdit m_qle;
    PG_TextBox *m_tb;
    PG_TextBox_textChanged m_changed;
};

#endif
