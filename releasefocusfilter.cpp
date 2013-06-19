/*******************************************
 *
 * Copyright © 2013 Léo Flaventin Hauchecorne
 *
 * This file is part of Ardoise.
 *
 * Ardoise is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ardoise is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 ********************************************/

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
