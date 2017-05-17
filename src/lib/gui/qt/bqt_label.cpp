#include "bqt_label.h"

#include "bqt_backend.h"

SOLOCAL Bqt_Label::Bqt_Label(PG_Label *l, const QString& text)
        : m_ql(text), m_l(l)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    QSize minSize = m_ql.minimumSizeHint();
    api->control.setContentSize(l, minSize.width(), minSize.height());
}

SOLOCAL void Bqt_Label::setParent(QWidget *parent)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    if (!parent) m_ql.hide();
    m_ql.setParent(parent);
    if (parent && api->control.shown(m_l)) m_ql.show();
}

SOLOCAL QWidget *Bqt_Label::widget()
{
    return &m_ql;
}

SOLOCAL void Bqt_Label::setText(const QString& text)
{
    m_ql.setText(text);
    QSize minSize = m_ql.minimumSizeHint();
    PG_qtBackend->privateApi->control.setContentSize(
                m_l, minSize.width(), minSize.height());
}

SOLOCAL_CDECL int Bqt_Label_create(PG_Label *l)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    QString text(api->label.text(l));
    Bqt_Label *bl = new Bqt_Label(l, text);
    api->setBackendObject(l, bl);
    return 1;
}

SOLOCAL_CDECL void Bqt_Label_setText(PG_Label *l, const char *text)
{
    Bqt_Label *bl = (Bqt_Label *)PG_qtBackend->privateApi->backendObject(l);
    bl->setText(QString(text));
}

SOLOCAL_CDECL void Bqt_Label_destroy(PG_Label *l)
{
    Bqt_Label *bl = (Bqt_Label *)PG_qtBackend->privateApi->backendObject(l);
    delete bl;
}
