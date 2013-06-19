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

#include "displaywidget.h"
#include <QLayout>
#include <QPushButton>
#include <QCloseEvent>
#include <QDialog>

DisplayWidget::DisplayWidget(QWidget * _w, QWidget * parent) :
QWidget(parent),
w(_w)
{
}

void DisplayWidget::closeEvent(QCloseEvent * e)
{
   w->setParent(0);//indispensable pour éviter que le widget soit détruit via Dialog::~Dialog()
   e->accept();
}

void DisplayWidget::create(const QString &title, QWidget *w, bool block)
{
   if(w->parentWidget())
   {
      w->parentWidget()->activateWindow();
      return;
   }
   if(block)
   {
      QDialog d;
      build(&d, title, w);
      d.exec();
      w->setParent(0);
   }
   else
   {
      DisplayWidget * d = new DisplayWidget(w);
      build(d, title, w);
      d->setAttribute(Qt::WA_DeleteOnClose);
      d->show();
   }
}

void DisplayWidget::build(QWidget *container, const QString &title, QWidget *w)
{
   w->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
   QVBoxLayout * vl = new QVBoxLayout;
   QHBoxLayout * hl = new QHBoxLayout;
   hl->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Maximum));
   QPushButton * b = new QPushButton(QObject::tr("Fermer"));
   b->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
   hl->addWidget(b);
   hl->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Maximum));
   vl->addWidget(w);
   vl->addLayout(hl);
   QObject::connect(b, SIGNAL(clicked()), container, SLOT(close()));
   container->setLayout(vl);
   container->setWindowTitle(title);
}
