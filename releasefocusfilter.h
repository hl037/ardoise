#ifndef RELEASEFOCUSFILTER_H
#define RELEASEFOCUSFILTER_H

#include <QObject>

class ReleaseFocusFilter : public QObject
{
   Q_OBJECT
public:
   explicit ReleaseFocusFilter(QObject *parent = 0);

protected:
   bool eventFilter(QObject * o, QEvent * ev);
   
signals:
   
public slots:
   
};

#endif // RELEASEFOCUSFILTER_H
