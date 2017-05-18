#ifndef BQT_MENUITEM_H
#define BQT_MENUITEM_H

#include <pocas/gui/private/backend.h>
#include <QObject>

class Bqt_Menu;

class Bqt_MenuItem : public QObject
{
    Q_OBJECT

public:
    Bqt_MenuItem(PG_MenuItem *mi, const QString &text);
    Bqt_Menu *subMenu() const;
    const QString &text() const;
    void setText(const QString &text);

public slots:
    void triggered(bool checked);

private:
    PG_MenuItem *m_mi;
    QString m_text;
};

#endif
