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
#include <QDir>
#include <QSet>

typedef struct
{
   QColor col1;
   QColor col2;
   double w1;
   double w2;
}brosse;

extern QDir home;

class QNetworkAccessManager;
class QNetworkReply;
class QTextBrowser;

class Version;

class MainWindow : public QMainWindow, private Ui::MainWindow {

Q_OBJECT

protected:
   static MainWindow * mainWindow;
public:
   static MainWindow * instance();
   void * operator new(std::size_t s);

public:
   static const char dtd[];

   void ini();
   void setShortcuts();

   MainWindow(QWidget *parent = 0);
   ~MainWindow();
   void saveCols(int pos);
   void saveWs(int pos);
   void restore(int pos);
   void erraseCols(int pos);
   void erraseWs(int pos);
   void erraseCols(void);
   void erraseWs(void);

   void savePal(const QString & path);
   void openPal(const QString & path);

   void saveConf();
   void openConf(const QString & path);

   virtual QMenu* createPopupMenu();

protected:
   Ardoise * scene;
   void changeEvent(QEvent *e);
   QString modeText();
   QColor col1;
   QColor col2;

   brosse brosses[27];

   bool block;

   virtual void closeEvent(QCloseEvent * e);
   virtual bool eventFilter(QObject *o, QEvent *ev);

   bool confirm(const QString &t, const QString & s);

   void notifUpdate(const Version & v);

   QNetworkAccessManager * netManager;
   int reqRemaining;

   QTextBrowser * help;

protected:
   /// NOTE : Hack temporaire pour l'ambiguité de la sauvegarde
   void saveImg(const QImage &img);

public slots:
   void setCol1();
   void setCol2();
   void setWeight1(double w);
   void setWeight2(double w);
   void savePal();
   void openPal();
   void clear();

   void save();
   inline void saveSelection(){saveImg(ardoise->getSelection());}
   inline void saveWindow(){saveImg(ardoise->getWindowImg());}
   inline void saveArdoise(){saveImg(ardoise->getImg());}

   void open();

   void swapMode();

   void showHelp();
   void showOpts();

   void fetchUpdates();

protected slots:
   void checkUpdates(QNetworkReply * r);
};

#endif // MAINWINDOW_H
