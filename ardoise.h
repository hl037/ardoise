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
#include "textinput.h"
#include "cursor.h"

class RectSelection;


class QPen;
class QImage;

//TODO ! v0.4.1.0 Agrandir automatiquement l'ardoise si dessin à l'extérieur en stockant les points, puis agrandir, dessiner ces points et copier ancienne img.

class Ardoise : public QWidget
{
Q_OBJECT
Q_PROPERTY(bool zoomWheel READ zoomWheel WRITE setZoomWheel)

public:

   typedef ArdoiseGlobal::Mode Mode;

   explicit Ardoise(QWidget *parent = 0);
   ~Ardoise();

   //TODO ne pas modifier l'image originale lors du Zoom
   void zoomTo(double fac, QPoint o);

   const QImage & getImg() const;
   RectSelection * select;

protected:
   QGraphicsScene * graphicsScene;

   QPoint imgOffset;

   Mode mode;

   bool typing;
   TextInput * textInput;
   QPoint textPos;
   QPoint textOffset;


protected:
   QCursor hiddenCur;
   ACursor * cur;
   QPen pen1;
   QPen pen2;
   QImage * img;
   QPoint lastPoint;
   QPoint mo;

   bool moveCursor;

   bool drawing;

   bool m_zoomWheel;

   /// retaille l'image : map = position (coordonnée de la nouvelle image) à laquelle sera dessinée l'ancienne image.
   void resizeImg(const QSize &newSize, QPoint map = QPoint(0,0));
   void fill();

   void paintEvent(QPaintEvent * e);
   void resizeEvent(QResizeEvent * e);
   void mouseMoveEvent(QMouseEvent *e);
   void mousePressEvent(QMouseEvent *e);
   void mouseReleaseEvent(QMouseEvent *e);
   void wheelEvent(QWheelEvent *e);

   void enterEvent(QEvent *){if(moveCursor) cur->show();}
   void leaveEvent(QEvent *){cur->hide();}

   inline QPoint getImgPoint(QPoint p){ return p+imgOffset; }

   void updateTIFont();

public:
   inline bool isTyping() { return typing; }
   void moveViewBy(const QPoint & offset);
   void lineTo(QPoint p, const QPen & pen);
   void pointTo(QPoint p, const QPen &pen);
   inline const QPen & getPen1() const {return pen1;}
   inline const QPen & getPen2() const {return pen2;}
   inline ACursor * getCursor(){return cur;}

   inline void setPen1(const QPen & newPen)
   {  pen1=newPen;   pen1.setCapStyle(Qt::RoundCap);   pen1.setJoinStyle(Qt::RoundJoin); updateTIFont();}
   inline void setPen2(const QPen & newPen)
      {  pen2=newPen;   pen2.setCapStyle(Qt::RoundCap);   pen2.setJoinStyle(Qt::RoundJoin); }


   void beginText(const QPoint & pos);
   void printText();

   inline Mode getMode(){return mode;}

   bool zoomWheel() const;

   /// Permet d'obtenir l'image contenue dans la sélection
   QImage getSelection();
   void setImage(const QImage & i);
signals:

public slots:
   void clear();
   // TODO ! v0.4.1.0 Possibilité de réinitialiser la sélection ainsi que de sélectionner toute la fenetre courante
   void affSelect(bool b);

   void endText();
   void nextLine();
   void cancelText();

   void swapMode();

   void setZoomWheel(bool activate);
};

#endif // ARDOISE_H
