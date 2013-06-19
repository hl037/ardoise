#include "releasefocusfilter.h"
#include <QKeyEvent>
#include <QtWidgets>

ReleaseFocusFilter::ReleaseFocusFilter(QObject *parent) :
QObject(parent)
{
}

bool ReleaseFocusFilter::eventFilter(QObject * o, QEvent * ev)
{
   QWidget * w = qobject_cast<QWidget *>(o);
   if(ev->type() != QEvent::KeyPress || !w) return false;

   QKeyEvent * e = static_cast<QKeyEvent*>(ev);

   switch(e->key())
   {
   case Qt::Key_Enter:
   case Qt::Key_Return:
   case Qt::Key_Escape:
      w->clearFocus();
      return true;
   default:
      break;
   }
   return false;
}
