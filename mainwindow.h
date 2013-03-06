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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "ardoise.h"
typedef struct
{
   QColor col1;
   QColor col2;
   double w1;
   double w2;
}brosse;


class mainWindow : public QMainWindow, private Ui::mainWindow {
    Q_OBJECT
public:
    mainWindow(QWidget *parent = 0);
    void saveCols(int pos);
    void saveWs(int pos);
    void restore(int pos);
    void erraseCols(int pos);
    void erraseWs(int pos);
    void erraseCols(void);
    void erraseWs(void);

protected:
   ardoise * scene;
   void changeEvent(QEvent *e);
   //void resizeEvent(QResizeEvent *e);
   void keyPressEvent(QKeyEvent *e);
   QColor col1;
   QColor col2;

   brosse brosses[26];

public slots:
   void setCol1();
   void setCol2();
   void setWeight1(double w);
   void setWeight2(double w);
   void savePal();
   void openPal();
};

#endif // MAINWINDOW_H
