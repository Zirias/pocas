#ifndef BQT_CONTROL_H
#define BQT_CONTROL_H

#include <QObject>

class Bqt_Control : public QObject
{
    Q_OBJECT

public:
    virtual void setShown(int shown) = 0;

protected:
    Bqt_Control();
};

#endif
