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
#include "mainwindow.h"


//TODO Rajouter la possibilité de dessiner des forme géométrique
//TODO faire un nouveau sélecteur de mode avec un truc du genre Alt+tab
//TODO Rajouter dans le fichier de sauvegarde l'état du zoommolette ainsi que le mode
//BUG : quand on clique sur un bouton dans le dock widget, la barre d'espace agit sur ce bouton au lieu de servir pour le mode



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainWindow w;
    a.installEventFilter(&w);
    w.show();
    return a.exec();
}
