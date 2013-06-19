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

#include <QtWidgets/QApplication>
#include <QTranslator>
#include "mainwindow.h"

//TODO Rajouter la possibilité de dessiner des forme géométrique
//TODO Faire un nouveau sélecteur de mode avec un truc du genre Alt+tab

QTranslator * translator;

int main(int argc, char *argv[])
{
   QTranslator t;
   translator = &t;
   QApplication a(argc, argv);
   a.installTranslator(translator);
   MainWindow w;
   a.installEventFilter(&w);
   w.show();
   return a.exec();
}
