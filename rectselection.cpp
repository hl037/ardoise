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
    setMinimumSize(SIZE_BOUTONx2+1,SIZE_BOUTONx2+1);
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

void RectSelection::resizeEvent(QResizeEvent * e)
{
   if(e->size().width()<SIZE_BOUTONx3)
   {
      b2->hide();
      b6->hide();
   }
   else
   {
      b2->move((width()-SIZE_BOUTON)/2, 0);
      b2->show();
      b6->move((width()-SIZE_BOUTON)/2,height()-SIZE_BOUTON);
      b6->show();
   }
   if(e->size().height()<SIZE_BOUTONx3)
   {
      b4->hide();
      b8->hide();
   }
   else
   {
      b4->move(width()-SIZE_BOUTON,(height()-SIZE_BOUTON)/2);
      b4->show();
      b8->move(0,(height()-SIZE_BOUTON)/2);
      b8->show();
   }
   b1->move(0,0);
   b3->move(width()-SIZE_BOUTON,0);
   b5->move(width()-SIZE_BOUTON,height()-SIZE_BOUTON);
   b7->move(0,height()-SIZE_BOUTON);
   s->setGeometry(SIZE_BOUTON,SIZE_BOUTON,width()-SIZE_BOUTON*2,height()-SIZE_BOUTON*2);
}

void RectSelection::mouseMoveEvent(QMouseEvent *e)
{
   if(change)
   {
      offset=e->globalPos()-o;
      (this->*this->RectSelection::proc)();
   }
   else e->ignore();

}

void RectSelection::mousePressEvent(QMouseEvent *e)
{
   if(e->button() == Qt::MiddleButton) e->ignore();
   else
   {
      vrect = geometry().adjusted(SIZE_BOUTON,SIZE_BOUTON,-SIZE_BOUTON,-SIZE_BOUTON);
      o=e->globalPos();
      change=1;
   }
}

void RectSelection::hg()
{
   setVGeometry(offset.x()+vrect.x(),offset.y()+vrect.y(),vrect.width()-offset.x(),vrect.height()-offset.y());
}

void RectSelection::h()
{
   setVGeometry(vrect.x(),offset.y()+vrect.y(),vrect.width(),vrect.height()-offset.y());
}

void RectSelection::hd()
{
   setVGeometry(vrect.x(),offset.y()+vrect.y(),vrect.width()+offset.x(),vrect.height()-offset.y());
}

void RectSelection::d()
{
   setVGeometry(vrect.x(),vrect.y(),vrect.width()+offset.x(),vrect.height());
}
void RectSelection::bd()
{
   setVGeometry(vrect.x(),vrect.y(),vrect.width()+offset.x(),vrect.height()+offset.y());
}

void RectSelection::b()
{
   setVGeometry(vrect.x(),vrect.y(),vrect.width(),vrect.height()+offset.y());
}

void RectSelection::bg()
{
   setVGeometry(offset.x()+vrect.x(),vrect.y(),vrect.width()-offset.x(),vrect.height()+offset.y());
}

void RectSelection::g()
{
   setVGeometry(offset.x()+vrect.x(),vrect.y(),vrect.width()-offset.x(),vrect.height());
}

void RectSelection::all()
{
   move(vrect.topLeft()+offset+QPoint(-SIZE_BOUTON,-SIZE_BOUTON));
}

void RectSelection::setVGeometry(const QRect &r)
{
   setGeometry(r.normalized().adjusted(-SIZE_BOUTON,-SIZE_BOUTON,SIZE_BOUTON,SIZE_BOUTON));
}

/*
void rectSelection::paintEvent(QPaintEvent *e)
{
   QPainter p(this);
   p.setBackground(QBrush(QColor(0,50,255,100)));
}*/

