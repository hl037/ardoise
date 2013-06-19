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

#ifndef RECTSELECTION_H
#define RECTSELECTION_H

#define SIZE_BOUTON 15
#define SIZE_BOUTONx2 30
#define SIZE_BOUTONx3 45

#include <QFrame>
#include <QRect>
#include "ardoise.h"
class airSelection : public QFrame
{
Q_OBJECT
public:
   airSelection(QWidget * parent) : QFrame(parent) {}

signals:
   void pressed();

protected:
   void mousePressEvent(QMouseEvent * e){emit pressed(); QWidget::mousePressEvent(e);}
   void mouseMoveEvent(QMouseEvent *e){QWidget::mouseMoveEvent(e);}
   void mouseReleaseEvent(QMouseEvent *e){QWidget::mouseReleaseEvent(e);}
};

#include "ui_rectselection.h"


class RectSelection : public QWidget, private Ui::rectSelection {
   Q_OBJECT
public:
   RectSelection(Ardoise *parent);

   void hg();
   void h();
   void hd();
   void d();
   void bd();
   void b();
   void bg();
   void g();
   void all();

   inline QRect getSelection() const {return QRect(x()+SIZE_BOUTON,y()+SIZE_BOUTON,width()-SIZE_BOUTONx2,height()-SIZE_BOUTONx2);}



protected:
   Ardoise * a;
   void (RectSelection::*proc)();
   bool change;
   QPoint o;
   QPoint oo;
   QPoint offset;
   int direction;
   QRect vrect;


   void changeEvent(QEvent *e);
   //void paintEvent(QPaintEvent *e);
   void resizeEvent(QResizeEvent *e);
   void mouseMoveEvent(QMouseEvent *e);
   void mousePressEvent(QMouseEvent *e);
   inline void mouseReleaseEvent(QMouseEvent *e) {change=0; e->ignore();}
   void setVGeometry(const QRect & r);
   inline void setVGeometry(int x, int y, int w, int h){setVGeometry(QRect(x,y,w,h));}
/*
   void enterEvent(QEvent *e);
   void leaveEvent(QEvent *e);*/

public slots:
   inline void sethg()
      {proc=&RectSelection::hg; direction=10;}

   inline void seth()
      {proc=&RectSelection::h; direction=8;}

   inline void sethd()
      {proc=&RectSelection::hd; direction=9;}

   inline void setd()
      {proc=&RectSelection::d; direction=1;}

   inline void setbd()
      {proc=&RectSelection::bd; direction=5;}

   inline void setb()
      {proc=&RectSelection::b; direction=4;}

   inline void setbg()
      {proc=&RectSelection::bg; direction=6;}

   inline void setg()
      {proc=&RectSelection::g; direction=2;}

   inline void setall()
      {proc=&RectSelection::all; direction=0;}


};

#endif // RECTSELECTION_H
