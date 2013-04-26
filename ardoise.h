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
 *
 ********************************************/

#ifndef ARDOISE_H
#define ARDOISE_H

#include <QWidget>
#include <QPen>
#include <QPoint>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QLineEdit>
#include "cursor.h"

class rectSelection;


class QPen;
class QImage;

class ardoise : public QWidget
{
Q_OBJECT
public:

   typedef ArdoiseGlobal::Mode Mode;

   explicit ardoise(QWidget *parent = 0);

   void resize(int rx, int ry, QPoint pos=QPoint(0,0)); //augmente la taille de l'image de |rx|*|ry| : le signe détermine si l'espace est ajouté "avant"(signe négatif) ou "apres"(signe positif) l'image' si nul, on deplace de la valeur indiquée par pos.

   //TODO ne pas modifier l'image originale lors du Zoom
   void zoomTo(double fac, QPoint o);

   const QImage & getImg() const;
   rectSelection * select;

protected:
   QGraphicsScene * graphicsScene;


   Mode mode;

   bool typing;
   QLineEdit * le;
   QPoint textPos;
   QPoint textOffset;


protected:
   QCursor hiddenCur;
   ACursor * cur;
   QPen pen1;
   QPen pen2;
   QImage img;
   QPoint lastPoint;

   bool moveCursor;

   bool dessin;

   void resizeImg(QImage * image, const QSize &newSize, QPoint map = QPoint(0,0));

   void paintEvent(QPaintEvent * e);
   void resizeEvent(QResizeEvent * e);
   void mouseMoveEvent(QMouseEvent *e);
   void mousePressEvent(QMouseEvent *e);
   void mouseReleaseEvent(QMouseEvent *e);
   void wheelEvent(QWheelEvent *e);

   void enterEvent(QEvent *){if(moveCursor) cur->show();}
   void leaveEvent(QEvent *){cur->hide();}


public:
   void lineTo(QPoint p, const QPen & pen);
   //TODO : Points de la même largeur que les lignes
   void pointTo(QPoint p, const QPen &pen);
   inline const QPen & getPen1() const {return pen1;}
   inline const QPen & getPen2() const {return pen2;}
   inline ACursor * getCursor(){return cur;}

   inline void setPen1(const QPen & newPen)
      {  pen1=newPen;   pen1.setCapStyle(Qt::RoundCap);   pen1.setJoinStyle(Qt::RoundJoin); }
   inline void setPen2(const QPen & newPen)
      {  pen2=newPen;   pen2.setCapStyle(Qt::RoundCap);   pen2.setJoinStyle(Qt::RoundJoin); }


   void beginText(const QPoint & pos);

   inline Mode getMode(){return mode;}
signals:

public slots:
   void clear();
   void affSelect(bool b);
   void save();
   void open();

   void swapMode();

   void endText();
};

#endif // ARDOISE_H
