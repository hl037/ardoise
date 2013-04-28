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

#include "ardoise.h"
#include "vue.h"
#include "cursor.h"
#define DEBUG

#include <QMouseEvent>
#include <QScrollBar>
#include <QPoint>


Vue::Vue(QWidget *parent) :
   QScrollArea(parent),
   m_zoomWheel(true)
{
   a = NULL;
}

void Vue::setWidget(Ardoise * widget)
{
   a = widget;
   QScrollArea::setWidget(widget);
}

void Vue::mousePressEvent(QMouseEvent *e)
{
   if(e->button()==Qt::MidButton)
   {
      o=a->pos();
      mo=e->pos();
      deplace=1;
   }
}

void Vue::fill()
{
   int rx=0,ry=0;
   if(a->x()>0) rx=-a->x();
   if(a->y()>0) ry=-a->y();
   a->resize(rx,ry);
   if(a->width()-a->x()<width()) rx=width()-a->width()+a->x(); else rx=0;
   if(a->height()-a->y()<height()) ry=height()-a->height()+a->y(); else ry=0;
   a->resize(rx,ry);

}

bool Vue::zoomWheel() const
{
   return m_zoomWheel;
}

void Vue::mouseMoveEvent(QMouseEvent *e)
{
   //cur->setGeometry(QRect(e->globalPos()-QPoint(50,50),e->globalPos()+QPoint(50,50)));
   //cur->setGeometry(QRect(e->pos()-QPoint(50,50),e->pos()+QPoint(50,50)));
   if((e->buttons()&Qt::MidButton) && deplace)
   {
      QPoint offset=e->pos()-mo;
      int rx=0, ry=0;
      int offX=offset.x(), offY=offset.y();

      if(offX!=0)
      {
         if(offX>0)
         {
            if(offX+a->pos().x()>0)
            {
               rx=-(offX+a->pos().x());
            }
         }
         else
            if(a->width()+a->pos().x()+offX<width())
            {
               rx=-(a->width()+a->pos().x()+offX-width()-128);
            }
      }


      if(offY!=0)
      {
         if(offY>0)
         {
            if(offY+a->pos().y()>0)
            {
               ry=-(offY+a->pos().y());
            }

         }
         else
            if(a->height()+a->pos().y()+offY<height())
            {
               ry=-(a->height()+a->pos().y()+offY-height()-128);
            }
      }

      a->resize(rx,ry,offset);
      mo=e->pos();

   }
}

void Vue::resizeEvent(QResizeEvent *e)
{
   fill();
   QScrollArea::resizeEvent(e);
}

void Vue::wheelEvent(QWheelEvent *)
{
   if(m_zoomWheel)
   {
      fill();
   }
}

void Vue::clear() //[slot]
{
   a->setGeometry(0,0,width(),height());
   a->clear();
}

void Vue::affSelect(bool b) //[slot]
{
   a->affSelect(b);
}

void Vue::save() //[slot]
{
   a->save();
}

void Vue::open() //[slot]
{
   a->open();
}

void Vue::setZoomWheel(bool activate)
{
   m_zoomWheel = activate;
   a->setZoomWheel(activate);
}
