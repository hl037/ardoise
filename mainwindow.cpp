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

#include "mainwindow.h"
#include "ardoise.h"
#include "cursor.h"
#include <cstdio>
#include <QColorDialog>
#include <QBrush>
#include <QPen>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <limits.h>
#include "flowlayout.h"

mainWindow::mainWindow(QWidget *parent) :
    QMainWindow(parent)
{
   setupUi(this);
   FlowLayout * f1 = new FlowLayout;
   FlowLayout * f2 = new FlowLayout;
   //QVBoxLayout * f3 = new QVBoxLayout;
   QVBoxLayout * f = new QVBoxLayout;

   f1->setMargin(2);
   f1->setHSpace(1);
   f1->setExpandsWidgets(true);
   f2->setMargin(2);
   f2->setHSpace(3);
   f->setMargin(0);




   f1->addWidget(couleur1);
   f1->addWidget(w1);
   f1->addWidget(couleur2);
   f1->addWidget(w2);
   f1->addWidget(statut);
   f2->addWidget(save);
   f2->addWidget(open);
   f2->addWidget(dispSel);
   f2->addWidget(erase);

   f->addItem(f1);
   f->addItem(f2);
   //f->addItem(f3);
   f->addStretch(1000);

   //doc->resize(doc->width(), 60);


   f1->setSizeConstraint(QLayout::SetMaximumSize);
   f2->setSizeConstraint(QLayout::SetMaximumSize);
   //f3->setSizeConstraint(QLayout::SetFixedSize);
   f->setSizeConstraint(QLayout::SetMaximumSize);
   //f->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::Maximum));


   doc->widget()->setLayout(f);

   scene = new ardoise(air);
   air->setWidget(scene);
   col1=QColor(0,0,0);
   col2=QColor(255,255,255);
   air->getArdoise()->getCursor()->setCol1(col1);
   air->getArdoise()->getCursor()->setCol2(col2);
   doc->resize(width(),300);
   setWeight1(w1->value());
   setWeight2(w2->value());
   air->getArdoise()->getCursor()->hide();
   for(int i = 0 ; i<26 ; ++i)
   {
      brosses[i].col1 = QColor();
      brosses[i].col2 = QColor();
      brosses[i].w1 = 0;
      brosses[i].w2 = 0;
   }
}

void mainWindow::changeEvent(QEvent *e)
{
   QMainWindow::changeEvent(e);
   switch (e->type()) {
   case QEvent::LanguageChange:
       retranslateUi(this);
       break;
   default:
      break;
   }
}

//void mainWindow::resizeEvent(QResizeEvent * e)
//{
//   air->setGeometry(2,2,surface->width()-4,surface->height()-4);
//}

void mainWindow::setCol1()  //[slot]
{
   air->getArdoise()->setPen1(QPen(QBrush(col1=QColorDialog::getColor(col1)), w1->value()));
   air->getArdoise()->getCursor()->setCol1(col1);
   char buf[40];
   sprintf(buf,"color: rgb(%i,%i,%i);",col1.red(),col1.green(),col1.blue());
   couleur1->setStyleSheet(QString(buf));
}

void mainWindow::setCol2()  //[slot]
{
   air->getArdoise()->setPen2(QPen(QBrush(col2=QColorDialog::getColor(col2)), w2->value()));
   air->getArdoise()->getCursor()->setCol2(col2);
   char buf[40];
   sprintf(buf,"color: rgb(%i,%i,%i);",col2.red(),col2.green(),col2.blue());
   couleur2->setStyleSheet(QString(buf));
}

void mainWindow::setWeight1(double w)  //[slot]
{
   air->getArdoise()->setPen1(QPen(QBrush(col1),w));
   air->getArdoise()->getCursor()->setD1(w);
}

void mainWindow::setWeight2(double w)  //[slot]
{
   air->getArdoise()->setPen2(QPen(QBrush(col2),w));
   air->getArdoise()->getCursor()->setD2(w);
}

void mainWindow::keyPressEvent(QKeyEvent *e)
{
   if(e->key()>=Qt::Key_A && e->key()<=Qt::Key_Z)
   {
      if(e->modifiers() & Qt::AltModifier && e->modifiers() & Qt::ControlModifier && e->key()==Qt::Key_S)
      {
         savePal();
         return;
      }

      if(e->modifiers() & Qt::AltModifier && e->modifiers() & Qt::ControlModifier && e->key()==Qt::Key_O)
      {
         openPal();
         return;
      }

      if(!e->modifiers())
      {
         restore(e->key()-Qt::Key_A);
         return;
      }

      if(e->modifiers() & Qt::AltModifier) saveCols(e->key()-Qt::Key_A);

      if(e->modifiers() & Qt::ShiftModifier) saveWs(e->key()-Qt::Key_A);

      return;
   }

   if(e->key()==Qt::Key_Backspace || e->key()==Qt::Key_Delete)
   {
      if(e->modifiers() & Qt::AltModifier)
      {
         erraseCols();
      }
      if(e->modifiers() & Qt::ShiftModifier)
      {
         erraseWs();
      }
      return;
   }

   if(e->modifiers()&Qt::ControlModifier)
   {
      if(e->key()==Qt::Key_Plus)
      {
         w1->setValue(w1->value()+1);
      }
      else if(e->key()==Qt::Key_Minus)
      {
         w1->setValue(w1->value()- (w1->value()>=1? 1:0));
      }
   }
   if(e->modifiers()&Qt::AltModifier)
   {
      if(e->key()==Qt::Key_Plus)
      {
         w2->setValue(w2->value()+1);
      }
      else if(e->key()==Qt::Key_Minus)
      {
         w2->setValue(w2->value()- (w2->value()>=1? 1:0));
      }
   }
}

void mainWindow::saveCols(int pos)
{
   if(brosses[pos].col1 == col1 && brosses[pos].col2 == col2)
   {
      erraseCols(pos);
      return;
   }
   brosses[pos].col1 = col1;
   brosses[pos].col2 = col2;
   statut->setText(QString("Couleurs Sauvegardées en [")+('A'+pos)+QString("] "));
}

void mainWindow::saveWs(int pos)
{
   if(brosses[pos].w1 == w1->value() && brosses[pos].w2 == w2->value())
   {
      erraseWs(pos);
      return;
   }
   brosses[pos].w1 = w1->value();
   brosses[pos].w2 = w2->value();
   statut->setText(QString("Épaisseurs Sauvegardées en [")+('A'+pos)+QString("] "));
}

void mainWindow::restore(int pos)
{
   if(brosses[pos].w1)
   {
      w1->setValue(brosses[pos].w1);
      air->getArdoise()->getCursor()->setD1(brosses[pos].w1);
   }
   if(brosses[pos].w2)
   {
      w2->setValue(brosses[pos].w2);
      air->getArdoise()->getCursor()->setD2(brosses[pos].w2);
   }
   char buf[40];
   if(brosses[pos].col1.isValid())
   {
      air->getArdoise()->setPen1(QPen(QBrush(col1 = brosses[pos].col1), w1->value()));
      air->getArdoise()->getCursor()->setCol1(col1);
      sprintf(buf,"color: rgb(%i,%i,%i);",col1.red(),col1.green(),col1.blue());
      couleur1->setStyleSheet(QString(buf));
   }
   if(brosses[pos].col2.isValid())
   {
      air->getArdoise()->setPen2(QPen(QBrush(col2 = brosses[pos].col2), w2->value()));
      air->getArdoise()->getCursor()->setCol2(col2);
      sprintf(buf,"color: rgb(%i,%i,%i);",col2.red(),col2.green(),col2.blue());
      couleur2->setStyleSheet(QString(buf));
   }

   statut->setText(QString("Brosses en [")+('A'+pos)+QString("] restorées"));
}

void mainWindow::erraseCols(int pos)
{
   brosses[pos].col1 = QColor();
   brosses[pos].col2 = QColor();
   statut->setText(QString("Couleurs en [")+('A'+pos)+QString("] Supprimées"));
}

void mainWindow::erraseWs(int pos)
{
   brosses[pos].w1 = 0;
   brosses[pos].w2 = 0;
   statut->setText(QString("Épaisseurs en [")+('A'+pos)+QString("] Supprimées"));
}

void mainWindow::erraseCols(void)
{
   for(uint i = 0 ; i<26 ; ++i)
   {
      brosses[i].col1 = QColor();
      brosses[i].col2 = QColor();
   }
   statut->setText(QString("Couleurs supprimées"));
}

void mainWindow::erraseWs(void)
{
   for(uint i = 0 ; i<26 ; ++i)
   {
      brosses[i].w1 = 0;
      brosses[i].w2 = 0;
   }
   statut->setText(QString("Épaisseurs supprimées"));
}

#include <stdio.h>

typedef unsigned char byte;
typedef unsigned int  uint;

void mainWindow::savePal()
{
   byte b[602];
   uint * i = (uint*) b;
   double * d;
#define O(offset) 4+offset*23
   i[0] = 1279348801; // "APAL"

   for(uint i = 0 ; i<26 ; ++i)
   {
      b[O(i)]= ((brosses[i].w1 || brosses[i].w2)<<4) + (brosses[i].col1.isValid() || brosses[i].col2.isValid());
      b[O(i)+1]=brosses[i].col1.red();
      b[O(i)+2]=brosses[i].col1.green();
      b[O(i)+3]=brosses[i].col1.blue();
      b[O(i)+4]=brosses[i].col2.red();
      b[O(i)+5]=brosses[i].col2.green();
      b[O(i)+6]=brosses[i].col2.blue();
      d = (double * )(b+O(i)+7);
      d[0] = brosses[i].w1;
      d[1] = brosses[i].w2;
   }

   QString chemin=QFileDialog::getSaveFileName(this, QString("Enregistrer la palette"), QString(), QString("Ardoise Palette (*.apal)"));
   FILE * f;
   f = fopen(chemin.toStdString().c_str(), "wb");
   /*
   for(uint i = 0 ; i<602 ; ++i)
   {
      fputc(b[i], f);
   };
   */
   fwrite(b,1,602,f);
   fclose(f);
   return;
}

void mainWindow::openPal()
{
   QString chemin=QFileDialog::getOpenFileName(this, QString("Ouvrir la palette"), QString(), QString("Ardoise Palette (*.apal);;Ardoise Palette ayant une autre extension (*.*)"));
   FILE * f = fopen(chemin.toStdString().c_str(), "rb");
   if(fgetc(f)!='A' || fgetc(f)!='P' || fgetc(f)!='A' || fgetc(f)!='L')
   {
      QMessageBox::critical(this, QString("Format inconnu"), QString("Le fichier ne correspond pas à une 'Ardoise Palette' valide."));
      return;
   }
   byte val;
   byte * d;
   for(uint i = 0 ; i<26 ; ++i)
   {
      val = fgetc(f);
      if(val & 0x1)
      {
         brosses[i].col1.setRed(fgetc(f));
         brosses[i].col1.setGreen(fgetc(f));
         brosses[i].col1.setBlue(fgetc(f));
         brosses[i].col2.setRed(fgetc(f));
         brosses[i].col2.setGreen(fgetc(f));
         brosses[i].col2.setBlue(fgetc(f));
      }
      else
      {
         fgetc(f);fgetc(f);fgetc(f);fgetc(f);fgetc(f);fgetc(f);
      }
      if(val & 0x10)
      {
         d = (byte *) &(brosses[i].w1);
         for(uint j = 0 ; j<8 ; ++j)
         {
            d[j]=fgetc(f);
         }
         d = (byte *) &(brosses[i].w2);
         for(uint j = 0 ; j<8 ; ++j)
         {
            d[j]=fgetc(f);
         }
      }
      else
      {
         for(uint j = 0 ; j<16 ; ++j)
         {
            fgetc(f);
         }
      }
   }
}
