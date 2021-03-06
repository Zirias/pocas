#include "bqt_label.h"
#include "bqt_backend.h"

SOLOCAL Bqt_Label::Bqt_Label(PG_Label *l, const QString& text)
        : m_ql(text), m_l(l)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    QSize minSize = m_ql.minimumSizeHint();
    PG_Size pgMinSize = PG_Size_init(minSize.width(), minSize.height());
    api->control.setContentSize(l, &pgMinSize);
    PG_LabelStyle style = api->label.style(l);
    Qt::Alignment align = 0;
    if (style & PG_LS_AlignBottom) align |= Qt::AlignBottom;
    if (style & PG_LS_AlignRight) align |= Qt::AlignRight;
    if (style & PG_LS_HCenter) align |= Qt::AlignHCenter;
    if (style & PG_LS_VCenter) align |= Qt::AlignVCenter;
    m_ql.setAlignment(align);
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
    PG_Size pgMinSize = PG_Size_init(minSize.width(), minSize.height());
    PG_qtBackend->privateApi->control.setContentSize(m_l, &pgMinSize);
}

SOLOCAL_CDECL int Bqt_Label_create(PG_Label *l)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    QString text = QString::fromUtf8(api->label.text(l));
    Bqt_Label *bl = new Bqt_Label(l, text);
    api->setBackendObject(l, bl);
    return 1;
}

SOLOCAL_CDECL void Bqt_Label_setText(PG_Label *l, const char *text)
{
    Bqt_Label *bl = (Bqt_Label *)PG_qtBackend->privateApi->backendObject(l);
    bl->setText(QString::fromUtf8(text));
}

SOLOCAL_CDECL void Bqt_Label_destroy(PG_Label *l)
{
    Bqt_Label *bl = (Bqt_Label *)PG_qtBackend->privateApi->backendObject(l);
    delete bl;
}
