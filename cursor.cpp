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

#include "cursor.h"
#include <QPainter>
#include <QGraphicsTextItem>

ACursor::ACursor(QWidget *parent) :
   QWidget(parent),
   t(new QGraphicsTextItem("T"))
{
   //setWindowFlags(Qt::SubWindow|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint);
   setAttribute(Qt::WA_TranslucentBackground);
   setMouseTracking(1);
   opt.initFrom(this);
}

void ACursor::paintEvent(QPaintEvent *)
{

   QPainter p(this);

   switch(m)
   {
   case ArdoiseGlobal::DRAWING_MODE:
      p.setBrush(QBrush(QColor(0,0,0,0)));
      p.setPen(p2);
      p.drawEllipse((width()-d2)/2,(height()-d2)/2,d2-1,d2-1);
      p.setPen(p1);
      p.drawEllipse((width()-d1)/2,(height()-d1)/2,d1-1,d1-1);
      p.setPen(p2);
      p.drawEllipse((width()-d2)/2,(height()-d2)/2,d2-1,d2-1);
      break;
   case ArdoiseGlobal::TEXT_MODE:
      //f.setStretch(d1/2);
      QFont f;
      f.setPixelSize(d1*8);
      t->setFont(f);
      t->setDefaultTextColor(p1.color());

      p.translate(width()/2,height()/2);
      t->paint(&p, &opt, 0);
   }

}
