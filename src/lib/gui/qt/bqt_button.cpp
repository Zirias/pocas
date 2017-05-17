#include "bqt_button.h"

#include "bqt_backend.h"

SOLOCAL Bqt_Button::Bqt_Button(PG_Button *b, const QString& text)
        : m_qb(text), m_b(b)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    connect(&m_qb, SIGNAL(clicked(bool)), this, SLOT(onClick(bool)));
    QSize minSize = m_qb.minimumSizeHint();
    api->control.setContentSize(b, minSize.width(), minSize.height());
}

SOLOCAL void Bqt_Button::setParent(QWidget *parent)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    if (!parent) m_qb.hide();
    m_qb.setParent(parent);
    if (parent && api->control.shown(m_b)) m_qb.show();
}

SOLOCAL QWidget *Bqt_Button::widget()
{
    return &m_qb;
}

SOLOCAL void Bqt_Button::setText(const QString& text)
{
    m_qb.setText(text);
    QSize minSize = m_qb.minimumSizeHint();
    PG_qtBackend->privateApi->control.setContentSize(
                m_b, minSize.width(), minSize.height());
}

SOLOCAL void Bqt_Button::onClick(bool checked)
{
    (void)checked;
    PG_qtBackend->privateApi->button.click(m_b);
}

SOLOCAL_CDECL int Bqt_Button_create(PG_Button *b)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    QString text(api->button.text(b));
    Bqt_Button *bb = new Bqt_Button(b, text);
    api->setBackendObject(b, bb);
    return 1;
}

SOLOCAL_CDECL void Bqt_Button_setText(PG_Button *b, const char *text)
{
    Bqt_Button *bb = (Bqt_Button *)PG_qtBackend->privateApi->backendObject(b);
    bb->setText(QString(text));
}

SOLOCAL_CDECL void Bqt_Button_destroy(PG_Button *b)
{
    Bqt_Button *bb = (Bqt_Button *)PG_qtBackend->privateApi->backendObject(b);
    delete bb;
}