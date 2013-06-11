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
#include <QPixmapCache>

ACursor::ACursor(QWidget *parent) :
   QWidget(parent),
   d1(2),
   d2(2),
   t(new QGraphicsTextItem("T"))
{
   setAttribute(Qt::WA_TranslucentBackground);
   setMouseTracking(1);
   opt.initFrom(this);
}

ACursor::~ACursor()
{
   t->deleteLater();
}

void ACursor::updateCursor()
{

   switch(m)
   {
   case ArdoiseGlobal::DRAWING_MODE:
   {
      int size = qMax(d1 + p1.width() * 2, d2 + p2.width() * 2);
      if(!size) size = 1;
      QPixmap pix(size, size);
      pix.fill(Qt::transparent);

      m_center.setX(size/2);
      m_center.setY(size/2);

      int r1 = d1/2;
      int r2 = d2/2;

      QPainter p;
      p.begin(&pix);
      p.setBrush(QBrush(QColor(0,0,0,0)));
      p.setPen(p2);
      p.drawEllipse(m_center, r2,r2);
      p.setPen(p1);
      p.drawEllipse(m_center, r1,r1);
      p.setPen(p2);
      p.drawEllipse(m_center, r2,r2);
      QPixmapCache::insert("Cursor", pix);
      p.end();

      resize(pix.size());
      break;
   }
   case ArdoiseGlobal::TEXT_MODE:
   {
      QFont f;
      f.setPixelSize(d1*8);
      t->setFont(f);
      t->setDefaultTextColor(p1.color());

      QPixmap pix(t->boundingRect().size().toSize());
      pix.fill(Qt::transparent);
      QPainter p(&pix);
      t->paint(&p, &opt, 0);
      p.end();
      QPixmapCache::insert("Cursor", pix);

      m_center.setX(0);
      m_center.setY(0);

      resize(pix.size());
      break;
   }
   default: ;
      QPixmap pix(2,2);
      pix.fill(Qt::black);
      QPixmapCache::insert("Cursor", pix);

      m_center.setX(pix.width()/2);
      m_center.setY(pix.height()/2);

      resize(pix.size());
      break;
   }
   repaint();
}

void ACursor::paintEvent(QPaintEvent *)
{
   QPainter p(this);

   QPixmap pix;
   QPixmapCache::find("Cursor", pix);
   p.drawPixmap(0,0,pix);
}
