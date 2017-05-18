#include "bqt_backend.h"
#include "bqt_menuitem.h"

#include "bqt_menu.h"
#include <QMenu>
#include <QMenuBar>

SOLOCAL Bqt_Menu::Bqt_Menu(PG_Menu *m)
        : m_m(m), m_items()
{ }


SOLOCAL void Bqt_Menu::add(Bqt_MenuItem *item)
{
    m_items.append(item);
}

SOLOCAL void Bqt_Menu::remove(Bqt_MenuItem *item)
{
    m_items.removeOne(item);
}

SOLOCAL QMenu *Bqt_Menu::createMenu(const QString &text)
{
    QMenu *menu = new QMenu(text);
    for (int i = 0; i < m_items.size(); ++i)
    {
        Bqt_MenuItem *item = m_items.at(i);
        Bqt_Menu *subMenu = item->subMenu();
        if (subMenu)
        {
            menu->addMenu(subMenu->createMenu(item->text()));
        }
        else
        {
            QAction *act = menu->addAction(item->text());
            QObject::connect(act, SIGNAL(triggered(bool)),
                    item, SLOT(triggered(bool)));
        }
    }
    return menu;
}

SOLOCAL QMenuBar *Bqt_Menu::createMenuBar()
{
    QMenuBar *bar = new QMenuBar();
    for (int i = 0; i < m_items.size(); ++i)
    {
        Bqt_MenuItem *item = m_items.at(i);
        Bqt_Menu *subMenu = item->subMenu();
        if (subMenu)
        {
            bar->addMenu(subMenu->createMenu(item->text()));
        }
        else
        {
            bar->addAction(item->text());
        }
    }
    return bar;
}

SOLOCAL_CDECL int Bqt_Menu_create(PG_Menu *m)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    Bqt_Menu *bm = new Bqt_Menu(m);
    api->setBackendObject(m, bm);
    return 1;
}

SOLOCAL_CDECL void Bqt_Menu_addItem(PG_Menu *m, PG_MenuItem *item)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    Bqt_Menu *bm = (Bqt_Menu *)api->backendObject(m);
    Bqt_MenuItem *bmi = (Bqt_MenuItem *)api->backendObject(item);
    bm->add(bmi);
}

SOLOCAL_CDECL void Bqt_Menu_removeItem(PG_Menu *m, PG_MenuItem *item)
{
    const PG_PrivateApi *api = PG_qtBackend->privateApi;
    Bqt_Menu *bm = (Bqt_Menu *)api->backendObject(m);
    Bqt_MenuItem *bmi = (Bqt_MenuItem *)api->backendObject(item);
    bm->remove(bmi);
}

SOLOCAL_CDECL void Bqt_Menu_destroy(PG_Menu *m)
{
    Bqt_Menu *bm = (Bqt_Menu *)PG_qtBackend->privateApi->backendObject(m);
    delete bm;
}
