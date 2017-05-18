#include "bqt_backend.h"

#include "bqt_menuitem.h"

SOLOCAL Bqt_MenuItem::Bqt_MenuItem(PG_MenuItem *mi, const QString &text)
        : m_mi(mi), m_text(text)
{ }

SOLOCAL Bqt_Menu *Bqt_MenuItem::subMenu() const
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    PG_Menu *sm = api->menuItem.subMenu(m_mi);
    if (!sm) return 0;
    return (Bqt_Menu *)api->backendObject(sm);
}

SOLOCAL const QString &Bqt_MenuItem::text() const
{
    return m_text;
}

SOLOCAL void Bqt_MenuItem::setText(const QString &text)
{
    m_text = text;
}

SOLOCAL void Bqt_MenuItem::triggered(bool checked)
{
    (void)checked;
    PG_qtBackend->privateApi->menuItem.select(m_mi);
}

SOLOCAL_CDECL int Bqt_MenuItem_create(PG_MenuItem *mi)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    QString text(api->menuItem.text(mi));
    Bqt_MenuItem *bmi = new Bqt_MenuItem(mi, text);
    api->setBackendObject(mi, bmi);
    return 1;
}

SOLOCAL_CDECL void Bqt_MenuItem_destroy(PG_MenuItem *mi)
{
    Bqt_MenuItem *bmi = (Bqt_MenuItem *)PG_qtBackend->privateApi->backendObject(mi);
    delete bmi;
}
