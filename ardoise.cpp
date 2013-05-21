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
#include "rectselection.h"

#include <QString>
#include <QImage>
#include <QPoint>
#include <QPainter>
#include <QRect>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSize>
#include <QRect>
#include <QFileDialog>
#include <QLabel>

#define FAC 1.25
#define FAC2 0.8


Ardoise::Ardoise(QWidget *parent) :
   QWidget(parent),
   imgOffset(0,0),
   mode(ArdoiseGlobal::DRAWING_MODE),
   typing(false),
   textOffset(0,0),
   m_zoomWheel(true)
{
   select = new RectSelection(this);
   select->hide();
   setPen1(QPen(QColor(0,0,0)));
   setPen2(QPen(QColor(255,255,255)));
   setMouseTracking(1);
   cur = new ACursor(this);
   cur->setMode(ArdoiseGlobal::DRAWING_MODE);
   QPixmap p(32,32);
   p.fill(QColor(0,0,0,0));
   hiddenCur = QCursor(p);
   setCursor(hiddenCur);
   moveCursor = 1;
   textInput = new TextInput(this);
   textInput->hide();

   connect(textInput, SIGNAL(accepted()), this, SLOT(endText()));
   connect(textInput, SIGNAL(nextLine()), this, SLOT(nextLine()));
   connect(textInput, SIGNAL(rejected()), this, SLOT(cancelText()));
   graphicsScene = new QGraphicsScene(this);
   fill();
}

const QImage & Ardoise::getImg() const
{
   return img;
}

void Ardoise::resizeImg(const QSize &newSize, QPoint map)
{
    if (img.size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(map, img);
    img = newImage;
}

void Ardoise::fill()
{
   QSize s(img.size());
   QPoint p(0,0);
   if(imgOffset.x()<0) // <-- est négatif!
   {
      s.rwidth()-=imgOffset.x();
      p.rx()-=imgOffset.x();
      imgOffset.setX(0);
   }
   if(imgOffset.y()<0)
   {
      s.rheight()-=imgOffset.y();
      p.ry()-=imgOffset.y();
      imgOffset.setY(0);
   }
   s.rwidth() += qMax(0, width()+imgOffset.x()-img.width());
   s.rheight() += qMax(0, height()+imgOffset.y()-img.height());

   resizeImg(s, p);
   update();
}

void Ardoise::lineTo(QPoint p, const QPen &pen)
{
   QPainter paint(&img);
   paint.setPen(pen);
   paint.drawLine(lastPoint, p);

   int rad = (pen.width() / 2) + 2;
   update(QRect(lastPoint, p).normalized().adjusted(-rad, -rad, +rad, +rad));
   lastPoint = p;
}

void Ardoise::pointTo(QPoint p, const QPen &pen)
{
   QPainter paint(&img);
   paint.setPen(pen);
   paint.drawPoint(p);
   lastPoint = p;
   update();
}

//TODO permettre d'annuler avec un appuie de la touche echape
//TODO ne dessiner qu'une seule fois le texte si on appuie sur entrée
void Ardoise::beginText(const QPoint &pos)
{
   if(typing)
   {
      printText();
   }

   typing = true;

   switch(mode)
   {
   case ArdoiseGlobal::DRAW_TEXT_MODE:
      textPos = pos;
      break;
   case ArdoiseGlobal::FLOATING_TEXT_MODE:
      textPos = pos+textOffset;
      break;
   default: ;
   }

   QFont f;
   f.setPixelSize(pen1.width()*8);
   textInput->setFont(f);
   textInput->move(pos-imgOffset);
   textInput->show();
   textInput->setFocus();
   textInput->selectAll();
}

void Ardoise::printText()
{
   QFont f;
   f.setPixelSize(pen1.width()*8);

   switch(mode)
   {
   case ArdoiseGlobal::DRAW_TEXT_MODE:
   {
      QGraphicsTextItem it(textInput->text());
      it.setFont(f);
      it.setDefaultTextColor(pen1.color());
      QPainter p(&img);
      p.translate(textPos);
      QStyleOptionGraphicsItem opt;
      opt.initFrom(this);
      it.paint(&p, &opt, 0);
      p.end();
      break;
   }
   case ArdoiseGlobal::FLOATING_TEXT_MODE:
   {
      QGraphicsTextItem * it = graphicsScene->addText(textInput->text(), f) ;
      it->setPos(textPos);
      it->setDefaultTextColor(pen1.color());
      break;
   }
   default: ;
   }
}

bool Ardoise::zoomWheel() const
{
   return m_zoomWheel;
}

QImage Ardoise::getSelection()
{
   QImage i(select->getSelection().size(),QImage::Format_RGB32);
   i.fill(0xFFFFFFFF);
   QPainter p(&i);
   p.drawImage(i.rect(),img,select->getSelection().translated(imgOffset));
   return i;
}

void Ardoise::setImage(const QImage &i)
{
   img = i;
   QSize s = (i.size()-size())/2;
   imgOffset.setX(s.width());
   imgOffset.setY(s.height());
   fill();
}

void Ardoise::endText()
{
   printText();
   textInput->hide();
   typing = false;
}

void Ardoise::nextLine()
{
   QPoint offset(0, textInput->lineHeight());
   beginText(textPos + offset);
}

void Ardoise::cancelText()
{
   textInput->hide();
   typing = false;
}

/*
void Ardoise::resize(int rx, int ry, QPoint pos)
{
   if(typing) cancelText();
   int nx,ny;
   QSize s(width()+qAbs(rx),height()+qAbs(ry));
   int mapX,mapY;
   if(rx<0)
   {
      nx=0;
      mapX=-rx;
   }
   else if(rx>0)
   {
      nx=x()+pos.x();
      mapX=0;
   }
   else { nx=x()+pos.x() ; mapX=0; }

   if(ry<0)
   {
      ny=0;
      mapY=-ry;
   }
   else if(ry>0)
   {
      ny=y()+pos.y();
      mapY=0;
   }
   else { ny=y()+pos.y() ; mapY=0; }

   //D("rx="<<rx<<" ry="<<ry<<" pos="<<pos<<" nx="<<nx<<" ny="<<ny<<"  mapX="<<mapX<<" mapY="<<mapY<<"  w="<<width()<<" h="<<height()<<"  x="<<x()<<" y="<<y())

   resizeImg(&img,s,QPoint(mapX,mapY));
   textOffset-=QPoint(mapX,mapY);
   QWidget::resize(s);
   move(QPoint(nx,ny));
   select->move(select->pos()+QPoint(mapX,mapY));
}*/

void Ardoise::zoomTo(double fac, QPoint o)
{
   QPoint p=o+(pos()-o)*fac;
#ifdef ONLY_FAST_TRANSPHORMATION
   img=img.scaledToHeight(height()*fac,Qt::FastTransformation);
#else
   if((height()|width())>5000)img=img.scaledToHeight(height()*fac,Qt::FastTransformation);
   else img=img.scaledToHeight(height()*fac,Qt::SmoothTransformation);
#endif
   QWidget::resize(size()*fac);
   move(p);
   QPainter painter(this);
   painter.drawImage(QPoint(0,0),img);
   select->setGeometry(QRect(select->pos()*fac,select->size()*fac));
}


void Ardoise::paintEvent(QPaintEvent * e)
{
   QPainter painter(this);
   QRect rectangle = e->rect();
   painter.drawImage(rectangle, img, rectangle.translated(imgOffset));
   QRect rt = rectangle.translated(textOffset);
   graphicsScene->render(&painter, rectangle, rt);
}

void Ardoise::resizeEvent(QResizeEvent * e)
{
   fill();
   QWidget::resizeEvent(e);
}

void Ardoise::mousePressEvent(QMouseEvent *e)
{
   grabMouse();
   QWidget::mousePressEvent(e);

   QPoint imgPoint = getImgPoint(e->pos());

   switch(mode)
   {
   case ArdoiseGlobal::DRAWING_MODE:
      if((e->button()==Qt::LeftButton || e->button()==Qt::RightButton) && moveCursor)
      {
         pointTo(imgPoint,e->button()==Qt::LeftButton? pen1:pen2);
         dessin=1;
         e->accept();
      }
      else e->ignore();
      break;
   case ArdoiseGlobal::DRAW_TEXT_MODE:
   case ArdoiseGlobal::FLOATING_TEXT_MODE:
      if(e->button()==Qt::LeftButton)
      {
         beginText(imgPoint);
         e->accept();
      }
      else e->ignore();
   }
   mo=e->pos();


}

void Ardoise::mouseMoveEvent(QMouseEvent *e)
{
   //cur->setGeometry(QRect(e->globalPos()-QPoint(50,50),e->globalPos()+QPoint(50,50)));
   if(moveCursor) cur->setGeometry(QRect(e->pos()-QPoint(50,50),e->pos()+QPoint(50,50)));
   QPoint imgPoint = getImgPoint(e->pos());
   if(dessin && moveCursor)
   {
      if((e->buttons() & Qt::LeftButton))
      {
         lineTo(imgPoint,pen1);
      }
      else
      {
         lineTo(imgPoint,pen2);
      }
   }
   else if(e->buttons() & Qt::MiddleButton)
   {
      QPoint offset=e->pos()-mo;
      mo = e->pos();
      moveViewBy(-offset);
   }
   e->ignore();
}

void Ardoise::mouseReleaseEvent(QMouseEvent *e)
{
   if(!e->buttons()) releaseMouse();
   QWidget::mouseReleaseEvent(e);
   QPoint imgPoint = getImgPoint(e->pos());
   if(dessin && moveCursor)
   {
      if(e->button()==Qt::LeftButton)  lineTo(imgPoint,pen1);
      else                             lineTo(imgPoint,pen2);
      dessin=0;
   }
   e->ignore();
}

void Ardoise::wheelEvent(QWheelEvent *)
{
   /* Temporary disabled, to provide a better zoom way
   if(m_zoomWheel)
   {
      if(e->delta()>0) zoomTo(FAC,e->pos());
      else zoomTo(FAC2,e->pos());
      QWidget::wheelEvent(e);
   }
   */
}

void Ardoise::moveViewBy(const QPoint &offset)
{
   imgOffset += offset;
   fill();
   select->move(select->pos()-offset);
}

void Ardoise::clear() //[slot]
{
   QImage i(size(), QImage::Format_RGB32);
   i.fill(0xFFFFFFFF);
   img=i;
   graphicsScene->clear();
   update();
}

void Ardoise::affSelect(bool b) //[slot]
{
   if(b)
   {
      select->show();
      moveCursor = 0;
      cur->hide();
      setCursor(Qt::ArrowCursor);
   }
   else
   {
      select->hide();
      moveCursor = 1;
      cur->show();
      setCursor(hiddenCur);
   }
}

void Ardoise::swapMode()
{
   if(mode == ArdoiseGlobal::DRAWING_MODE)
   {
      mode = ArdoiseGlobal::TEXT_MODE;
      cur->setMode(ArdoiseGlobal::TEXT_MODE);
   }
   else
   {
      mode = ArdoiseGlobal::DRAWING_MODE;
      cur->setMode(ArdoiseGlobal::DRAWING_MODE);
   }
   typing = false;
}

void Ardoise::setZoomWheel(bool activate)
{
   m_zoomWheel = activate;
}

