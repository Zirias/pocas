#ifndef BQT_MENU_H
#define BQT_MENU_H

#include <pocas/gui/private/backend.h>

#include <QList>

class QMenu;
class QMenuBar;
class Bqt_MenuItem;

class Bqt_Menu
{
public:
    Bqt_Menu(PG_Menu *m);
    void add(Bqt_MenuItem *item);
    void remove(Bqt_MenuItem *item);
    QMenu *createMenu(const QString &text);
    QMenuBar *createMenuBar();

private:
    PG_Menu *m_m;
    QList<Bqt_MenuItem *> m_items;
};

#endif
