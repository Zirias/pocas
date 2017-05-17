#ifndef BQT_CONTROL_H
#define BQT_CONTROL_H

#include <QObject>

class QWidget;

class Bqt_Control : public QObject
{
    Q_OBJECT

public:
    virtual void setShown(int shown);
    virtual void setParent(QWidget *parent) = 0;
    virtual void setGeometry(const QRect& rect);
    virtual QWidget *widget() = 0;

protected:
    Bqt_Control();
};

#endif
