#include "bqt_textbox.h"
#include "bqt_backend.h"

#define textBoxWidth 100

SOLOCAL Bqt_TextBox::Bqt_TextBox(PG_TextBox *tb, PG_TextBox_textChanged changed)
        : m_qle(), m_tb(tb), m_changed(changed)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    connect(&m_qle, SIGNAL(textChanged(QString)),
            this, SLOT(textChanged(QString)));
    QSize size = m_qle.sizeHint();
    api->control.setContentSize(tb, textBoxWidth, size.height());
}

SOLOCAL void Bqt_TextBox::setParent(QWidget *parent)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    if (!parent) m_qle.hide();
    m_qle.setParent(parent);
    if (parent && api->control.shown(m_tb)) m_qle.show();
}

SOLOCAL QWidget *Bqt_TextBox::widget()
{
    return &m_qle;
}

SOLOCAL void Bqt_TextBox::focus()
{
    Bqt_Control::focus();
    m_qle.selectAll();
}

SOLOCAL void Bqt_TextBox::setText(const QString &text)
{
    m_qle.setText(text);
}

SOLOCAL void Bqt_TextBox::textChanged(const QString &text)
{
    m_changed(m_tb, text.toUtf8().data());
}

SOLOCAL_CDECL int Bqt_TextBox_create(PG_TextBox *tb, PG_TextBox_textChanged changed)
{
    Bqt_TextBox *btb = new Bqt_TextBox(tb, changed);
    PG_qtBackend->privateApi->setBackendObject(tb, btb);
    return 1;
}

SOLOCAL_CDECL void Bqt_TextBox_setText(PG_TextBox *tb, const char *text)
{
    Bqt_TextBox *btb = (Bqt_TextBox *)PG_qtBackend->privateApi->backendObject(tb);
    btb->setText(QString::fromUtf8(text));
}

SOLOCAL_CDECL void Bqt_TextBox_destroy(PG_TextBox *tb)
{
    Bqt_TextBox *btb = (Bqt_TextBox *)PG_qtBackend->privateApi->backendObject(tb);
    delete btb;
}
