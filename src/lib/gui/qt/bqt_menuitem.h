#ifndef BQT_MENUITEM_H
#define BQT_MENUITEM_H

#include <pocas/gui/private/backend.h>
#include <QString>

class Bqt_Menu;

class Bqt_MenuItem
{
public:
    Bqt_MenuItem(PG_MenuItem *mi, const QString &text);
    Bqt_Menu *subMenu() const;
    const QString &text() const;
    void setText(const QString &text);

private:
    PG_MenuItem *m_mi;
    QString m_text;
};

#endif
