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

#ifndef VUE_H
#define VUE_H

#include <QScrollArea>
#include "cursor.h"

class Ardoise;
class rectSelection;
class ACursor;

class Vue : public QScrollArea
{
   Q_OBJECT
   Q_PROPERTY(bool zoomWheel READ zoomWheel WRITE setZoomWheel)

public:
   explicit Vue(QWidget *parent = 0);
   void setWidget(Ardoise * widget);


protected:
   Ardoise * a;
   QPoint o;
   QPoint mo;
   bool deplace;
   bool m_zoomWheel;

   void mousePressEvent(QMouseEvent *e);
   void mouseMoveEvent(QMouseEvent *e);
   void resizeEvent(QResizeEvent *e);
   void wheelEvent(QWheelEvent *e);

private:
   void fill();

public:
   inline Ardoise * getArdoise(){return a;}

   bool zoomWheel() const;

public slots:
   void clear();
   void affSelect(bool b);
   void save();
   void open();

   void setZoomWheel(bool activate);

};

#endif // VUE_H
