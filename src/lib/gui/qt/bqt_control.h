#ifndef BQT_CONTROL_H
#define BQT_CONTROL_H

#include <QObject>

class QWidget;
class QRect;

class Bqt_Control : public QObject
{
    Q_OBJECT

public:
    virtual void setShown(bool shown);
    virtual void setEnabled(bool enabled);
    virtual void setParent(QWidget *parent) = 0;
    virtual void setGeometry(const QRect& rect);
    virtual void focus();
    virtual QWidget *widget() = 0;

protected:
    Bqt_Control();
};

#endif
