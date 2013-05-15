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
#include "vue.h"

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
   mode(ArdoiseGlobal::DRAWING_MODE),
   textOffset(0,0),
   m_zoomWheel(true)
{
   select = new rectSelection(this);
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
   le = new QLineEdit(this);
   le->hide();

   connect(le, SIGNAL(editingFinished()), this, SLOT(endText()));
   graphicsScene = new QGraphicsScene(this);
   //zoom=1;
}

const QImage & Ardoise::getImg() const
{
   return img;
}

void Ardoise::resizeImg(QImage * image, const QSize &newSize, QPoint map)
//! [19] //! [20]
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(map, *image);
    *image = newImage;
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
   update();
}

//TODO permettre d'annuler avec un appuie de la touche echape
//TODO ne dessiner qu'une seule fois le texte si on appuie sur entrée
void Ardoise::beginText(const QPoint &pos)
{
   typing = true;
   textPos = pos+textOffset;
   QFont f;
   f.setPixelSize(pen1.width()*8);
   le->setFont(f);
   le->move(pos);
   le->show();
   le->setFocus();
}

bool Ardoise::zoomWheel() const
{
   return m_zoomWheel;
}

void Ardoise::endText()
{

   QFont f;
   f.setPixelSize(pen1.width()*8);

   switch(mode)
   {
   case ArdoiseGlobal::DRAW_TEXT_MODE:
   {
      QGraphicsTextItem it(le->text());
      it.setFont(f);
      it.setDefaultTextColor(pen1.color());
      QPainter p(&img);
      p.translate(lastPoint);
      QStyleOptionGraphicsItem opt;
      opt.initFrom(this);
      it.paint(&p, &opt, 0);
      p.end();
      break;
   }
   case ArdoiseGlobal::FLOATING_TEXT_MODE:
   {
      QGraphicsTextItem * it = graphicsScene->addText(le->text(), f) ;
      it->setPos(textPos);
      it->setDefaultTextColor(pen1.color());
      break;
   }
   default: ;
   }


   le->hide();
}


void Ardoise::resize(int rx, int ry, QPoint pos)
{
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

   //D("rx="<<rx<<" ry="<<ry<<" pos="<<pos<<" nx="<<nx<<" ny="<<ny<<"  mapX="<<mapX<<" mapY="<<mapY<<"  w="<<width()<<" h="<<height()<<"  x="<<x()<<" y="<<y())/**/

   resizeImg(&img,s,QPoint(mapX,mapY));
   textOffset-=QPoint(mapX,mapY);
   QWidget::resize(s);
   move(QPoint(nx,ny));
   select->move(select->pos()+QPoint(mapX,mapY));
}

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
   painter.drawImage(rectangle, img, rectangle);
   QRect rt = rectangle.translated(textOffset);
   graphicsScene->render(&painter, rectangle, rt);
}

void Ardoise::resizeEvent(QResizeEvent * e)
{
   if(width()>img.width() || height()>img.height())
   {
      int newWidth = qMax(width() + 128, img.width());
      int newHeight = qMax(height() + 128, img.height());
      resizeImg(&img, QSize(newWidth, newHeight));
      update();
   }
   QWidget::resizeEvent(e);
}

//BUG : Quand le curseur sort de la zone fenetre de l'application en cours de dessin, il passe en effacement automatiquement
void Ardoise::mousePressEvent(QMouseEvent *e)
{
   QWidget::mousePressEvent(e);

   switch(mode)
   {
   case ArdoiseGlobal::DRAWING_MODE:
      if((e->button()==Qt::LeftButton || e->button()==Qt::RightButton) && moveCursor)
      {
         lastPoint=e->pos();
         pointTo(lastPoint,e->button()==Qt::LeftButton? pen1:pen2);
         dessin=1;
         e->accept();
      }
      else e->ignore();
      break;
   case ArdoiseGlobal::DRAW_TEXT_MODE:
   case ArdoiseGlobal::FLOATING_TEXT_MODE:
      if(e->button()==Qt::LeftButton)
      {
         beginText(lastPoint=e->pos());
         e->accept();
      }
      else e->ignore();
   }


}

void Ardoise::mouseMoveEvent(QMouseEvent *e)
{
   //cur->setGeometry(QRect(e->globalPos()-QPoint(50,50),e->globalPos()+QPoint(50,50)));
   if(moveCursor) cur->setGeometry(QRect(e->pos()-QPoint(50,50),e->pos()+QPoint(50,50)));
   if(dessin && moveCursor)
   {
      if((e->buttons() & Qt::LeftButton))
      {
         lineTo(e->pos(),pen1);
      }
      else
      {
         lineTo(e->pos(),pen2);
      }
   }
   e->ignore();
}

void Ardoise::mouseReleaseEvent(QMouseEvent *e)
{
   QWidget::mouseReleaseEvent(e);
   if(dessin && moveCursor)
   {
      if(e->button()==Qt::LeftButton)  lineTo(e->pos(),pen1);
      else                             lineTo(e->pos(),pen2);
      dessin=0;
   }
   e->ignore();
}

void Ardoise::wheelEvent(QWheelEvent *e)
{
   if(m_zoomWheel)
   {
      if(e->delta()>0) zoomTo(FAC,e->pos());
      else zoomTo(FAC2,e->pos());
      QWidget::wheelEvent(e);
   }
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

void Ardoise::save() //[slot]
{
   QString filter;
   QString chemin=QFileDialog::getSaveFileName(this, tr("Enregistrer l'image"), QString(), tr("Bitmap Windows (*.bmp);;Joint Photographic Experts Group JPEG (*.jpg *.jpeg);;Portable Network Graphics PNG (*.png);;Portable Pixmap (*.ppm);;Tagged Image File Format (*.tiff);;X11 Bitmap (*.xbm);;X11 Pixmap (*.xpm)"),&filter);
   if(chemin.isEmpty()) return;
   QFileInfo info(chemin);
   QString suffix = info.suffix();
   const char * format = 0;
#ifndef ARDOSIE_AUTO_SUFFIXE
   if(!suffix.isEmpty())
   {
#endif

   if( filter == "Bitmap Windows (*.bmp)" )
   {
      format = "BMP";
#ifdef ARDOSIE_AUTO_SUFFIXE
      if(suffix != "bmp" && ( ( chemin[0]!='"' && chemin[0]!='\'' ) || ( chemin[chemin.size()-1]!='"' && chemin[chemin.size()-1]!='\'' ) ) )
      {
         chemin += ".bmp";
      }
#endif
   }
   if( filter == "Joint Photographic Experts Group JPEG (*.jpg *.jpeg)" )
   {
      format = "JPG";
#ifdef ARDOSIE_AUTO_SUFFIXE
      if( suffix != "jpg" && ( ( chemin[0]!='"' && chemin[0]!='\'' ) || ( chemin[chemin.size()-1]!='"' && chemin[chemin.size()-1]!='\'' ) ) )
      {
         chemin += ".jpg";
      }
#endif
   }
   if( filter == "Portable Network Graphics PNG (*.png)" )
   {
      format = "PNG";
#ifdef ARDOSIE_AUTO_SUFFIXE
      if( suffix != "png" && ( ( chemin[0]!='"' && chemin[0]!='\'' ) || ( chemin[chemin.size()-1]!='"' && chemin[chemin.size()-1]!='\'' ) ) )
      {
         chemin += ".png";
      }
#endif
   }
   if( filter == "Portable Pixmap (*.ppm)" )
   {
      format = "PPM";
#ifdef ARDOSIE_AUTO_SUFFIXE
      if( suffix != "ppm" && ( ( chemin[0]!='"' && chemin[0]!='\'' ) || ( chemin[chemin.size()-1]!='"' && chemin[chemin.size()-1]!='\'' ) ) )
      {
         chemin += ".ppm";
      }
#endif
   }
   if( filter == "Tagged Image File Format (*.tiff)" )
   {
      format = "TIFF";
#ifdef ARDOSIE_AUTO_SUFFIXE
      if( suffix != "tiff" && ( ( chemin[0]!='"' && chemin[0]!='\'' ) || ( chemin[chemin.size()-1]!='"' && chemin[chemin.size()-1]!='\'' ) ) )
      {
         chemin += ".tiff";
      }
#endif
   }
   if( filter == "X11 Bitmap (*.xbm)" )
   {
      format = "XBM";
#ifdef ARDOSIE_AUTO_SUFFIXE
      if( suffix != "xbm" && ( ( chemin[0]!='"' && chemin[0]!='\'' ) || ( chemin[chemin.size()-1]!='"' && chemin[chemin.size()-1]!='\'' ) ) )
      {
         chemin += ".xbm";
      }
#endif
   }
   if( filter == "X11 Pixmap (*.xpm)" )
   {
      format = "XPM";
#ifdef ARDOSIE_AUTO_SUFFIXE
      if( suffix != "xpm" && ( ( chemin[0]!='"' && chemin[0]!='\'' ) || ( chemin[chemin.size()-1]!='"' && chemin[chemin.size()-1]!='\'' ) ) )
      {
         chemin += ".xpm";
      }
#endif
   }

#ifndef ARDOSIE_AUTO_SUFFIXE
   }
#endif
   QImage saveImg(select->getSelection().size(),QImage::Format_RGB32);
   saveImg.fill(0xFFFFFFFF);
   QPainter p(&saveImg);
   p.drawImage(saveImg.rect(),img,select->getSelection());
   saveImg.save(chemin,format,80);
}

void Ardoise::open() //[slot]
{

   QString chemin=QFileDialog::getOpenFileName(this, tr("Ouvrir l'image"), QString(), tr("Bitmap Windows (*.bmp);;Joint Photographic Experts Group JPEG (*.jpg *.jpeg);;Portable Network Graphics PNG (*.png);;Portable Pixmap (*.ppm);;Tagged Image File Format (*.tiff);;X11 Bitmap (*.xbm);;X11 Pixmap (*.xpm);;Graphic Interchange Format GIF (*.gif);;Portable Bitmap (*.pbm);;Portable Graymap (*.pgm)"));

   QFile f(chemin);
   if(f.exists())
   {
      QImage loadImg(chemin,0);
      img=loadImg;
      setGeometry(img.rect());
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
}

void Ardoise::setZoomWheel(bool activate)
{
   m_zoomWheel = activate;
}

