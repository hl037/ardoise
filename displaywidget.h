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

#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>

class DisplayWidget : public QWidget
{

   Q_OBJECT

public:
   static void create(const QString &title, QWidget *w, bool block = 0);

protected:
   static void build(QWidget * container, const QString &title, QWidget *w);
   QWidget * w;
   explicit DisplayWidget(QWidget *_w, QWidget *parent = 0);
   void closeEvent(QCloseEvent *e);
   
signals:
   
public slots:
   
};

#endif // DISPLAYWIDGET_H
