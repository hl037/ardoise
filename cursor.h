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

#ifndef CURSOR_H
#define CURSOR_H

#include <QWidget>
#include <QPen>
#include <QMouseEvent>
#include <QStyleOptionGraphicsItem>

namespace ArdoiseGlobal
{

enum Mode
{
   DRAWING_MODE = 1,
   TEXT_MODE = 2,
   DRAW_TEXT_MODE = 2,
   FLOATING_TEXT_MODE = 3
};

}

class QGraphicsTextItem;

class ACursor : public QWidget
{

Q_OBJECT

  typedef ArdoiseGlobal::Mode Mode;

protected:
   QPen p1,p2;
   int d1,d2;

   Mode m;
   QPixmap imgCur;
   QPoint m_center;
   QGraphicsTextItem * t;
   QStyleOptionGraphicsItem opt;

public:

   ACursor(QWidget *parent = 0);
   ~ACursor();

protected:
   void updateCursor();
   void updateCursor(QPixmap & pm);
   void paintEvent(QPaintEvent *);
   void mousePressEvent(QMouseEvent *e){e->ignore();}
   void mouseMoveEvent(QMouseEvent *e){e->ignore();}
   void mouseReleaseEvent(QMouseEvent *e){e->ignore();}

public:
   inline QPoint center() { return m_center; }

signals:

public slots:
   inline void setD1(int d){d1 = (d>=2)? d:2; updateCursor();}
   inline void setCol1(QColor col){p1 = QPen(QBrush(col),1,Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); updateCursor();}
   inline void setD2(int d){d2 = (d>=2)? d:2; updateCursor();}
   inline void setCol2(QColor col){p2 = QPen(QBrush(col),1,Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); updateCursor();}

   inline void setMode(Mode _m){m = _m; updateCursor();}
};

#endif // CURSOR_H
