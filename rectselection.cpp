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

#include "rectselection.h"

#include <QMouseEvent>
#include <QPainter>
#include <QBrush>
#include <QColor>


RectSelection::RectSelection(Ardoise * parent) :
    QWidget(parent)
{
    a = parent;
    setupUi(this);
}

void RectSelection::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void RectSelection::resizeEvent(QResizeEvent *)
{
   b1->move(0,0);
   b2->move((width()-SIZE_BOUTON)/2, 0);
   b3->move(width()-SIZE_BOUTON,0);
   b4->move(width()-SIZE_BOUTON,(height()-SIZE_BOUTON)/2);
   b5->move(width()-SIZE_BOUTON,height()-SIZE_BOUTON);
   b6->move((width()-SIZE_BOUTON)/2,height()-SIZE_BOUTON);
   b7->move(0,height()-SIZE_BOUTON);
   b8->move(0,(height()-SIZE_BOUTON)/2);
   s->setGeometry(SIZE_BOUTON,SIZE_BOUTON,width()-SIZE_BOUTON*2,height()-SIZE_BOUTON*2);
   //D("***RESIZE EVENT***  x="<<x()<<"  y="<<y()<<"    w="<<width()<<"  h="<<height())
}

void RectSelection::mouseMoveEvent(QMouseEvent *e)
{
   if(change)
   {
      offset=e->pos()-o;
      o=e->pos();
      (this->*this->RectSelection::proc)();
   }
   else e->ignore();

}

void RectSelection::mousePressEvent(QMouseEvent *e)
{
   if(e->button() == Qt::MiddleButton) e->ignore();
   else
   {
      o=e->pos();
      oo=o;
      change=1;
   }
}
/*
void rectSelection::enterEvent(QEvent *e)
{
   a->hideCur();
   a->setMouseTracking(0);
}

void rectSelection::leaveEvent(QEvent *e)
{
   a->showCur();
   a->setMouseTracking(1);
}
*/
void RectSelection::hg()
{
   setGeometry(o.x()+x(),o.y()+y(),width()-o.x(),height()-o.y());
}

void RectSelection::h()
{
   setGeometry(x(),o.y()+y(),width(),height()-o.y());
}

void RectSelection::hd()
{
   setGeometry(x(),o.y()+y(),width()+offset.x(),height()-o.y());
}

void RectSelection::d()
{
   setGeometry(x(),y(),width()+offset.x(),height());
}

void RectSelection::bd()
{
   setGeometry(x(),y(),width()+offset.x(),height()+offset.y());
}

void RectSelection::b()
{
   setGeometry(x(),y(),width(),height()+offset.y());
}

void RectSelection::bg()
{
   setGeometry(o.x()+x(),y(),width()-o.x(),height()+offset.y());
}

void RectSelection::g()
{
   setGeometry(o.x()+x(),y(),width()-o.x(),height());
}

void RectSelection::all()
{
   move(o.x()+x()-oo.x(),o.y()+y()-oo.y());
}

/*
void rectSelection::paintEvent(QPaintEvent *e)
{
   QPainter p(this);
   p.setBackground(QBrush(QColor(0,50,255,100)));
}*/

