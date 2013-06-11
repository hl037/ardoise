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
#include <QTextBrowser>
#include <QResource>
#include <limits.h>
#include "flowlayout.h"
#include <QXmlDefaultHandler>
#include <QXmlSimpleReader>
#include <QMessageBox>
#include <QShortcut>
#include <QStack>
#include <QDir>
#include <QSpacerItem>
#include <QLocale>
#include <iostream>
#include <fstream>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "json/ljsonp.hpp"
#include "options.h"
#include "optionswidget.h"
MainWindow *MainWindow::mainWindow = nullptr;

MainWindow *MainWindow::instance()
{
   if(!mainWindow) throw "no MainWindows instance";
   return mainWindow;
}

void *MainWindow::operator new(std::size_t s)
{
   if(mainWindow != nullptr) throw "a MainWindows instance already exists";
   return mainWindow = static_cast<MainWindow*>(::operator  new (s));
}


void displayWidget(const QString & title, QWidget * w, QWidget * parent = 0)
{
   QDialog d(parent);
   w->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
   d.setWindowTitle(title);
   QVBoxLayout * vl = new QVBoxLayout;
   QHBoxLayout * hl = new QHBoxLayout;
   hl->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Maximum));
   QPushButton * b = new QPushButton(QObject::tr("Fermer"));
   b->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
   hl->addWidget(b);
   hl->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Maximum));
   vl->addWidget(w);
   vl->addLayout(hl);
   QObject::connect(b, SIGNAL(clicked()), &d, SLOT(accept()));
   d.setLayout(vl);
   d.exec();
   w->setParent(0);//indispensable pour éviter que le widget soit détruit via Dialog::~Dialog()
}

const char MainWindow::dtd[] = R"(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE xapal [

<!ELEMENT xapal (pal)? >
<!ELEMENT pal (brush)* >
<!ELEMENT brush (col|weigth)* >
<!ELEMENT col EMPTY >
<!ELEMENT weigth EMPTY >


<!ATTLIST brush key CDATA #REQUIRED>

<!ATTLIST brush key CDATA #REQUIRED>

<!ATTLIST col i CDATA #REQUIRED>
<!ATTLIST col r CDATA #REQUIRED>
<!ATTLIST col g CDATA #REQUIRED>
<!ATTLIST col b CDATA #REQUIRED>

<!ATTLIST weigth i CDATA #REQUIRED>
<!ATTLIST weigth w CDATA #REQUIRED>

]>
)";

bool supportOptionsFiles;

QDir home;

void MainWindow::ini()
{
   //========================
   //    Options
   //========================
   Options::create();
   //------------------------
   //    Buit-in langugaes
   //------------------------
   LanguageOption * langs = new LanguageOption("lang", 0,
      QList<QLocale>{QLocale("fr"), QLocale("en")},
      QT_TRANSLATE_NOOP("OptionsWidget", "Langage"),
      QT_TRANSLATE_NOOP("OptionsWidget", "Langage dans lequel sera l'interface, ainsi que l'aide."),
      "general");
   Options::addOption(langs);
   //------------------------
   //    Auto-updates
   //------------------------
   Option * autoCheckUpdates = new Option("auto-check-updates", true,
      QT_TRANSLATE_NOOP("OptionsWidget", "Vérification des mise à jour au démarrage"),
      QT_TRANSLATE_NOOP("OptionsWidget", "À chaque démarrage, l'ardoise va rechercher sur le net si des mises à jour sont disponible et affiche un message vous invitant à les télécharger si c'est le cas."),
      "general");
   Options::addOption(autoCheckUpdates);
   //------------------------
   //    Repos-Check-updates
   //------------------------
   StringListOption * checkUrls = new StringListOption("check-urls",
   QVariant(QStringList{}),
      "",
      "Liste des urls où on peut trouver ardoise.json",
      "general");
   Options::addOption(checkUrls);


   supportOptionsFiles = false;
#if defined __linux__
   home = QDir::home();
   home.mkpath(".config/ardoise");
   if(!home.cd(".config/ardoise"))
   {
      std::cerr<<"can't create or access to configuration files, use default"<<std::endl;
      return;
   }

   if(home.exists("default.xapal"))
   {
      openPal(home.absoluteFilePath("default.xapal"));
   }
   else
   {
      savePal(home.absoluteFilePath("default.xapal"));
   }
   if(home.exists("last.xapal"))
   {
      openPal(home.absoluteFilePath("last.xapal"));
   }
   if(home.exists("conf.json"))
   {
      openConf(home.absoluteFilePath("conf.json"));
   }
   if(autoCheckUpdates->getValue().toBool()) fetchUpdates();
   supportOptionsFiles = true;
#elif defined _WIN32
   home = QDir::current();
   home.mkpath("conf");
   if(!home.cd("conf"))
   {
      std::cerr<<"can't create or access to configuration files, use default"<<std::endl;
      return;
   }

   if(home.exists("default.xapal"))
   {
      openPal(home.absoluteFilePath("default.xapal"));
   }
   else
   {
      savePal(home.absoluteFilePath("default.xapal"));
   }
   if(home.exists("last.xapal"))
   {
      openPal(home.absoluteFilePath("last.xapal"));
   }
   if(home.exists("conf.json"))
   {
      openConf(home.absoluteFilePath("conf.json"));
   }
   supportPalRecov = true;
#else
#warning "This build won't support palette recovery"
#endif
}
void MainWindow::setShortcuts()
{
   new QShortcut(QKeySequence(QString("Ctrl+S")), this, SLOT(save()), 0, Qt::ApplicationShortcut);
   new QShortcut(QKeySequence(QString("Ctrl+O")), this, SLOT(open()), 0, Qt::ApplicationShortcut);
   new QShortcut(QKeySequence(Qt::Key_1), this, SLOT(setCol1()), 0, Qt::ApplicationShortcut);
   new QShortcut(QKeySequence(Qt::Key_2), this, SLOT(setCol2()), 0, Qt::ApplicationShortcut);
}

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   block(false),
   netManager(new QNetworkAccessManager(this)),
   reqRemaining(0),
   help(new QTextBrowser)
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




   f1->addWidget(couleur1_pb);
   f1->addWidget(w1_sb);
   f1->addWidget(couleur2_pb);
   f1->addWidget(w2_sb);
   f1->addWidget(statut_lab);
   f2->addWidget(save_pb);
   f2->addWidget(open_pb);
   f2->addWidget(dispSel_pb);
   f2->addWidget(erase_pb);
   f2->addWidget(swapMode_pb);
   f2->addWidget(update_pb);
   f2->addWidget(help_pb);
   f2->addWidget(showOpts_pb);
   //f2->addWidget(zoomWheel);

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

   col1=QColor(0,0,0);
   col2=QColor(255,255,255);
   ardoise->getCursor()->setCol1(col1);
   ardoise->getCursor()->setCol2(col2);
   doc->resize(width(),300);
   setWeight1(w1_sb->value());
   setWeight2(w2_sb->value());
   ardoise->getCursor()->hide();
   for(int i = 0 ; i<26 ; ++i)
   {
      brosses[i].col1 = QColor();
      brosses[i].col2 = QColor();
      brosses[i].w1 = 0;
      brosses[i].w2 = 0;
   }
   swapMode_pb->setText(tr("Mode dessin, changer pour texte"));
   connect(swapMode_pb, SIGNAL(clicked()), this, SLOT(swapMode()));
   connect(netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(checkUpdates(QNetworkReply*)));
   ini();
   setShortcuts();
}

MainWindow::~MainWindow()
{
   help->deleteLater();
   Options::destroy();
}

void MainWindow::changeEvent(QEvent *e)
{
   QMainWindow::changeEvent(e);
   switch (e->type()) {
   case QEvent::LanguageChange:
      retranslateUi(this);
      swapMode_pb->setText(modeText());
      break;
   default:
      break;
   }
}

QString MainWindow::modeText()
{
   switch(ardoise->getMode())
   {
   case ArdoiseGlobal::DRAWING_MODE:
      return tr("Mode dessin, changer pour texte");
      break;
   case ArdoiseGlobal::TEXT_MODE:
      return tr("Mode texte, changer pour dessin");
      break;
   default: ;
   }
   return QString();
}

//void mainWindow::resizeEvent(QResizeEvent * e)
//{
//   air->setGeometry(2,2,surface->width()-4,surface->height()-4);
//}

void MainWindow::setCol1()  //[slot]
{
   ardoise->setPen1(QPen(QBrush(col1=QColorDialog::getColor(col1)), w1_sb->value()));
   ardoise->getCursor()->setCol1(col1);
   char buf[40];
   sprintf(buf,"color: rgb(%i,%i,%i);",col1.red(),col1.green(),col1.blue());
   couleur1_pb->setStyleSheet(QString(buf));
}

void MainWindow::setCol2()  //[slot]
{
   ardoise->setPen2(QPen(QBrush(col2=QColorDialog::getColor(col2)), w2_sb->value()));
   ardoise->getCursor()->setCol2(col2);
   char buf[40];
   sprintf(buf,"color: rgb(%i,%i,%i);",col2.red(),col2.green(),col2.blue());
   couleur2_pb->setStyleSheet(QString(buf));
}

void MainWindow::setWeight1(double w)  //[slot]
{
   ardoise->setPen1(QPen(QBrush(col1),w));
   ardoise->getCursor()->setD1(w);
}

void MainWindow::setWeight2(double w)  //[slot]
{
   ardoise->setPen2(QPen(QBrush(col2),w));
   ardoise->getCursor()->setD2(w);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
   QMainWindow::closeEvent(e);
   if(e->isAccepted() && supportOptionsFiles)
   {
      savePal(home.absoluteFilePath("last.xapal"));
   }
}

bool MainWindow::eventFilter(QObject * o, QEvent *ev)
{
   if(ev->type() != QEvent::KeyPress) return false;
   QKeyEvent * e = static_cast<QKeyEvent*>(ev);

   bool b = false;

   if(!isActiveWindow() || o->inherits("QWidgetWindow") || o->inherits("QLineEdit") || o->inherits("QDialog") || o->inherits("QAbstractSpinBox")) return false;

   //Palette
   if(e->key()>=Qt::Key_A && e->key()<=Qt::Key_Z)
   {
      if(e->modifiers() & Qt::AltModifier && e->modifiers() & Qt::ControlModifier && e->key()==Qt::Key_S)
      {
         savePal();
         return true;
      }

      if(e->modifiers() & Qt::AltModifier && e->modifiers() & Qt::ControlModifier && e->key()==Qt::Key_O)
      {
         openPal();
         return true;
      }

      if(!e->modifiers())
      {
         restore(e->key()-Qt::Key_A);
         return true;
      }

      if(e->modifiers() & Qt::AltModifier)
      {
         saveCols(e->key()-Qt::Key_A);
         b = true;
      }
      if(e->modifiers() & Qt::ShiftModifier)
      {
         saveWs(e->key()-Qt::Key_A);
         b = true;
      }
      if(b) return true;
   }

   //Palette Resets
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
      return true;
   }

   //SwapMode
   if(e->key() == Qt::Key_Space)
   {
      swapMode();
      return true;
   }

   //Pinceau
   if(e->modifiers()&Qt::ControlModifier)
   {
      if(e->key()==Qt::Key_Plus)
      {
         w1_sb->setValue(w1_sb->value()+1);
         b = true;
      }
      else if(e->key()==Qt::Key_Minus)
      {
         w1_sb->setValue(w1_sb->value()- (w1_sb->value()>=1? 1:0));
         b = true;
      }
   }
   if(e->modifiers()&Qt::AltModifier)
   {
      if(e->key()==Qt::Key_Plus)
      {
         w2_sb->setValue(w2_sb->value()+1);
         b = true;
      }
      else if(e->key()==Qt::Key_Minus)
      {
         w2_sb->setValue(w2_sb->value()- (w2_sb->value()>=1? 1:0));
         b = true;
      }
   }
   return b;
}

bool MainWindow::confirm(const QString & t, const QString &s)
{
   return QMessageBox::warning(this, t, s, QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok;
}
void MainWindow::saveCols(int pos)
{
   if(brosses[pos].col1 == col1 && brosses[pos].col2 == col2)
   {
      erraseCols(pos);
      return;
   }
   brosses[pos].col1 = col1;
   brosses[pos].col2 = col2;
   statut_lab->setText(tr("Couleurs Sauvegardées %1").arg(QString("[")+('A'+pos)+']'));
}

void MainWindow::saveWs(int pos)
{
   if(brosses[pos].w1 == w1_sb->value() && brosses[pos].w2 == w2_sb->value())
   {
      erraseWs(pos);
      return;
   }
   brosses[pos].w1 = w1_sb->value();
   brosses[pos].w2 = w2_sb->value();
   statut_lab->setText(tr("Épaisseurs Sauvegardées %1").arg(QString("[")+('A'+pos)+']'));
}

void MainWindow::restore(int pos)
{
   if(brosses[pos].w1)
   {
      w1_sb->setValue(brosses[pos].w1);
      ardoise->getCursor()->setD1(brosses[pos].w1);
   }
   if(brosses[pos].w2)
   {
      w2_sb->setValue(brosses[pos].w2);
      ardoise->getCursor()->setD2(brosses[pos].w2);
   }
   char buf[40];
   if(brosses[pos].col1.isValid())
   {
      ardoise->setPen1(QPen(QBrush(col1 = brosses[pos].col1), w1_sb->value()));
      ardoise->getCursor()->setCol1(col1);
      sprintf(buf,"color: rgb(%i,%i,%i);",col1.red(),col1.green(),col1.blue());
      couleur1_pb->setStyleSheet(QString(buf));
   }
   if(brosses[pos].col2.isValid())
   {
      ardoise->setPen2(QPen(QBrush(col2 = brosses[pos].col2), w2_sb->value()));
      ardoise->getCursor()->setCol2(col2);
      sprintf(buf,"color: rgb(%i,%i,%i);",col2.red(),col2.green(),col2.blue());
      couleur2_pb->setStyleSheet(QString(buf));
   }
   statut_lab->setText(tr("Brosses en %1 restorées").arg(QString("[")+('A'+pos)+']'));
}

void MainWindow::erraseCols(int pos)
{
   brosses[pos].col1 = QColor();
   brosses[pos].col2 = QColor();
   statut_lab->setText(tr("Couleurs en %1 Supprimées").arg(QString("[")+('A'+pos)+']'));
}

void MainWindow::erraseWs(int pos)
{
   brosses[pos].w1 = 0;
   brosses[pos].w2 = 0;
   statut_lab->setText(tr("Épaisseurs en %1 Supprimées").arg(QString("[")+('A'+pos)+']'));
}

void MainWindow::erraseCols(void)
{
   if(!confirm(tr("Réinitialiser les couleurs de la palette"), tr("Êtes-vous sûr de vouloir réinitialiser toutes les couleurs de la palette?"))) return;
   for(uint i = 0 ; i<26 ; ++i)
   {
      brosses[i].col1 = QColor();
      brosses[i].col2 = QColor();
   }
   statut_lab->setText(tr("Couleurs supprimées"));
}

void MainWindow::erraseWs(void)
{
   if(!confirm(tr("Réinitialiser les épaisseurs de la palette"), tr("Êtes-vous sûr de vouloir réinitialiser toutes les épaisseurs de la palette?"))) return;
   for(uint i = 0 ; i<26 ; ++i)
   {
      brosses[i].w1 = 0;
      brosses[i].w2 = 0;
   }
   statut_lab->setText(tr("Épaisseurs supprimées"));
}

#include <stdio.h>

typedef unsigned char byte;
typedef unsigned int  uint;

static const QString replaceb =
"    <brush key=\"%1\">\n"
"%2"
"    </brush>\n";
static const QString replacec =
"      <col i=\"%1\" r=\"%2\" g=\"%3\" b=\"%4\"/>\n";
static const QString replacew =
"      <weigth i=\"%1\" w=\"%2\"/>\n";

static QString br2xml(const brosse & b)
{
   QString s="";
   if(b.col1.isValid())
   {
      s += replacec.arg("1",
         QString::number(b.col1.red()),
         QString::number(b.col1.green()),
         QString::number(b.col1.blue()) );
   }
   if(b.col2.isValid())
   {
      s += replacec.arg("2",
         QString::number(b.col2.red()),
         QString::number(b.col2.green()),
         QString::number(b.col2.blue()) );
   }
   if(b.w1)
   {
      s += replacew.arg("1", QString::number(b.w1));
   }
   if(b.w2)
   {
      s += replacew.arg("2", QString::number(b.w2));
   }
   return s;
}

void MainWindow::savePal()
{
   QString path=QFileDialog::getSaveFileName(this, tr("Enregistrer la palette"), QString(), QString("Ardoise Palette (*.apal);;XML Ardoise Palette(*.xapal)"));
   if(!path.isEmpty()) savePal(path);
}

void MainWindow::savePal(const QString & path)
{
   QString ext = QFileInfo(path).suffix();

   //Sauvegarde de la brosse actuelle
   brosses[26].w1 = w1_sb->value();
   brosses[26].w2 = w2_sb->value();
   brosses[26].col1 = col1;
   brosses[26].col2 = col2;

   if(ext == "xapal")
   {
      FILE * f;
      f = fopen(path.toStdString().c_str(), "wb");
      fwrite(dtd,1,sizeof(dtd)-1,f);
      const char prefix[] =
      "<xapal>\n"
      "  <pal>\n";
      fwrite(prefix,1,sizeof(prefix)-1,f);

      QString s = br2xml(brosses[26]);

      if(!s.isEmpty())
      {
         s = replaceb.arg("default", s);
         fwrite(s.toStdString().c_str(),1,s.size(),f);
      }

      for(uint i = 0 ; i<26 ; ++i)
      {
         QString s = br2xml(brosses[i]);

         if(!s.isEmpty())
         {
            s = replaceb.arg(QString('a'+i), s);
            fwrite(s.toStdString().c_str(),1,s.size(),f);
         }
      }
      const char suffixe[] =
      "  </pal>\n"
      "</xapal>\n";
      fwrite(suffixe,1,sizeof(suffixe)-1,f);
      fclose(f);
   }
   else
   {
      byte b[625];
      uint * i = (uint*) b;
      double * d;
   #define O(offset) 4+offset*23
      i[0] = 1279348801; // "APAL"

      for(uint i = 0 ; i<27 ; ++i)
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

      FILE * f;
      f = fopen(path.toStdString().c_str(), "wb");
      /*
      for(uint i = 0 ; i<602 ; ++i)
      {
         fputc(b[i], f);
      };
      */
      fwrite(b,1,602,f);
      fclose(f);
   }
   return;
}

class XmlHandler : public QXmlDefaultHandler
{
   //Q_OBJECT
protected:
   brosse * br;
   enum State
   {
      ROOT,
      XAPAL,
      PAL,
      BRUSH,
      COL,
      WEIGTH,
      UNKNOWN
   };

   struct Status
   {
      State s;
      QVariant v;
      Status(const State & _s, const QVariant & _v) :
      s(_s),
      v(_v)
      {}
      Status() :
      s(UNKNOWN),
      v(0)
      {}
   };

   QStack<Status> stack;

public:
   XmlHandler(brosse * brosses):
   QXmlDefaultHandler(),
   br(brosses)
   {
      stack.push(Status(ROOT, 1));
   }

   virtual ~XmlHandler()
   {}

   virtual bool startocument()
   {
      return true;
   }

   virtual bool startElement(const QString &, const QString &localName, const QString &, const QXmlAttributes &atts)
   {
      const Status & st = stack.top();
      switch(st.s)
      {
      case ROOT:
      {
         if(localName.toLower() != "xapal")
         {
            return false;
         }
         stack.push(Status(XAPAL, 1));
         return true;
      }
      case XAPAL:
      {
         if(localName.toLower() != "pal")
         {
            return false;
         }
         stack.push(Status(PAL, 1));
         return true;
      }
      case PAL:
      {
         if(localName.toLower() != "brush")
         {
            return false;
         }
         QString i = atts.value("key");
         if(i.isEmpty() || i == "default") stack.push(Status(BRUSH, 26));
         else
         {
            int n = i[0].unicode()-'a';
            if(n<0 || n>25 || i.size()>1) return false;
            stack.push(Status(BRUSH, n));
         }
         return true;
      }
      case BRUSH:
      {
         int key = st.v.toInt();
         if(localName == "col")
         {
            bool ok = false;

            int i = atts.value("i").toInt(&ok);
            if(!ok || (i!=1 && i!=2)) return false;

            int r = atts.value("r").toInt(&ok);
            if(!ok || r<0 || r>0xFF) return false;

            int g = atts.value("g").toInt(&ok);
            if(!ok || g<0 || g>0xFF) return false;

            int b = atts.value("b").toInt(&ok);
            if(!ok || b<0 || b>0xFF) return false;

            if(i==1) br[key].col1.setRgb(r,g,b);
            else     br[key].col2.setRgb(r,g,b);
            stack.push(Status(COL, 1));
            return true;
         }
         if(localName == "weigth")
         {
            bool ok = false;

            int i = atts.value("i").toInt(&ok);
            if(!ok || (i!=1 && i!=2)) return false;

            double w = atts.value("w").toDouble(&ok);
            if(!ok || w<=0) return false;

            if(i==1) br[key].w1 = w;
            else     br[key].w2 = w;
            stack.push(Status(WEIGTH, 1));
            return true;
         }
         return false;
      }
      default:
         return false;
      }
   }

   virtual bool endElement(const QString &, const QString &, const QString &)
   {
      stack.pop();
      return !stack.isEmpty();
   }

};

void MainWindow::openPal()
{
   QString path=QFileDialog::getOpenFileName(this, tr("Ouvrir la palette"), QString(), QString("Ardoise Palette (*.apal);;XML Ardoise Palette(*.xapal);;Ardoise Palette ayant une autre extension (*.*)"));
   openPal(path);
}

void MainWindow::clear()
{
   if(!confirm(tr("Réinitialiser le dessin"), tr("Êtes-vous sûr de vouloir effacer tout le dessin?"))) return;
   ardoise->clear();
}


void MainWindow::openPal(const QString & path)
{
   QFileInfo info(path);
   if(!info.exists()) return;
   QString ext = QFileInfo(path).suffix();

   if(ext == "xapal")
   {
      QXmlSimpleReader r;
      XmlHandler h(brosses);
      r.setContentHandler(&h);
      QFile f(path);
      r.parse(QXmlInputSource(&f));
   }
   else
   {
      FILE * f = fopen(path.toStdString().c_str(), "rb");
      if(fgetc(f)!='A' || fgetc(f)!='P' || fgetc(f)!='A' || fgetc(f)!='L')
      {
         QMessageBox::critical(this, tr("Format inconnu"), tr("Le fichier ne correspond pas à une 'Ardoise Palette' valide."));
         return;
      }
      int tmp;
      byte val;
      byte * d;
      for(uint i = 0 ; i<27 ; ++i)
      {
         val = tmp = fgetc(f);
         if(!tmp) break;
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
   restore(26);
}

void MainWindow::save() //[slot]
{
   QString filter;
   QString chemin=QFileDialog::getSaveFileName(this, tr("Enregistrer l'image"), QString(), QString("Bitmap Windows (*.bmp);;Joint Photographic Experts Group JPEG (*.jpg *.jpeg);;Portable Network Graphics PNG (*.png);;Portable Pixmap (*.ppm);;Tagged Image File Format (*.tiff);;X11 Bitmap (*.xbm);;X11 Pixmap (*.xpm)"),&filter);
   if(chemin.isEmpty()) return;
   QFileInfo info(chemin);
   QString suffix = info.suffix();
   const char * format = 0;
#ifndef ARDOSIE_AUTO_SUFFIXE
   if(suffix.isEmpty())
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
   ardoise->getSelection().save(chemin,format,80);
}


void MainWindow::open() //[slot]
{

   QString chemin=QFileDialog::getOpenFileName(this, tr("Ouvrir l'image"), QString(), QString("Bitmap Windows (*.bmp);;Joint Photographic Experts Group JPEG (*.jpg *.jpeg);;Portable Network Graphics PNG (*.png);;Portable Pixmap (*.ppm);;Tagged Image File Format (*.tiff);;X11 Bitmap (*.xbm);;X11 Pixmap (*.xpm);;Graphic Interchange Format GIF (*.gif);;Portable Bitmap (*.pbm);;Portable Graymap (*.pgm)"));

   QFile f(chemin);
   if(f.exists())
   {
      ardoise->setImage(QImage(chemin,0));
   }
}

void MainWindow::swapMode()
{
   ardoise->swapMode();
   swapMode_pb->setText(modeText());
}

void MainWindow::showHelp()
{
   QResource r(QString(":/help_%1.html")
      .arg(Options::getOption_cast<LanguageOption>("lang")->currentLang())
      );
   if(r.isValid())
   {
      QByteArray b( reinterpret_cast< const char* >( r.data() ), r.size() );
      help->setText(QString(b));
   }
   else
   {
      QResource r(QString(":/help.html"));
      QByteArray b( reinterpret_cast< const char* >( r.data() ), r.size() );
      help->setText(QString(b));
   }
   displayWidget(tr("Aide Ardoise"), help);
}

void MainWindow::showOpts()
{
   OptionsWidget * opts = Options::optionsWidget();
   opts->exec();
}

void MainWindow::saveConf()
{
   if(supportOptionsFiles)
   {
      QFile f(home.filePath("conf.json"));
      if(f.open(QFile::WriteOnly))
      {
         f.write(Options::saveConf());
      }
      f.close();
   }
}

void MainWindow::openConf(const QString &path)
{
   QFile f(path);
   if(f.open(QFile::ReadOnly))
   {
      Options::readConf(f.readAll());
      f.close();
   }
}

struct Version
{
   int n[4];
   QString desc;
   QString install;
   bool operator<(const Version & v)
   {
      for(int i=0 ; i<3 ; ++i)
      {
         if(n[i] != v.n[i]) return n[i]<v.n[i];
      }
      return n[3]<v.n[3];
   }

   Version(QString id, const QString & _desc = QString(), const QString & _install = QString()) :
   n{0,0,0,0},
   desc(_desc),
   install(_install)
   {
      if(id[0] != 'v') return;
      id = id.mid(1);
      QStringList l = id.split('.');
      if(l.size() != 4) return;
      int * ptr = n;
      for(QString s : l)
      {
         *(ptr++) = s.toInt();
      }
   }

   Version(const Version & cp) :
   n{cp.n[0],cp.n[1],cp.n[2],cp.n[3]}
   { }

   Version operator= (const Version & cp)
   {
      memcpy(n, cp.n, sizeof(n));
      desc = cp.desc;
      install = cp.install;
      return *this;
   }

   bool operator== (const Version & v)
   {
      return n[0] == v.n[0] &&
         n[1] == v.n[1] &&
         n[2] == v.n[2] &&
         n[3] == v.n[3];
   }

   bool operator != (const Version & v)
   {
      return ! (*this == v);
   }

   static Version self;
};

#include "version.h"

Version Version::self(QString(VERSION));
#ifndef SYSTEM

#if defined(_WIN32)
#define SYSTEM "win32"

#elif defined(__linux__)
#define SYSTEM "linux"

#endif
#endif

void MainWindow::fetchUpdates()
{
   if(reqRemaining) return;
   QStringList checkUrls = Options::get("check-urls").toStringList();
   reqRemaining = checkUrls.size();
   for(QString s : checkUrls)
   {
      netManager->get(QNetworkRequest(QUrl(s)));
   }
}


void MainWindow::checkUpdates(QNetworkReply * r)
{

   static Version max = Version::self;

   if(r->error() == QNetworkReply::NoError)
   {
      using namespace std;
      using namespace ljsoncpp;

      QByteArray data = r->readAll();
      stringstream stream(QString(data).toStdString());


      bool ok;
      Object * o;
      Array * links;
      Array * versions;

      string id;
      Object * o2;
      QString desc;
      QString install;
      Array * dls;



      //Pour l'instant, on cherche juste la dernière versiondiponible

      Value * root = Parser::parse(stream);
      if(!root) goto END;
      //assignement root
      o = root->get<Object*>(&ok);
      if(!ok) goto FAIL;

      links = o->getAttr<Array*>("check-urls", &ok);
      if(ok)
      {
         StringListOption * cklist = Options::getOption_cast<StringListOption>("check-urls");
         QStringList l = cklist->getStringList();
         for(Value * v : *links)
         {
            string str = v->get<string>(&ok);
            if(ok)
            {
               QString s(str.c_str());

               if(!l.contains(s))
               {
                  l<<s;
                  cklist->addValue(s);
                  ++reqRemaining;
                  netManager->get(QNetworkRequest(QUrl(s)));
               }
            }
         }
         saveConf();
      }

      versions = o->getAttr<Array*>("Ardoise", &ok);
      if(!ok) goto FAIL;

      for(Value * v : *versions)
      {
         o = v->get<Object*>(&ok);
         if(!ok) continue;

         id = o->getAttr<string>("id", &ok);
         if(!ok) continue;

         o2 = o->getAttr<Object*>("desc");
         desc = (o2
         ? QString( o2->getAttr<std::string>( Options::getOption_cast<LanguageOption>("lang")->currentLang().toStdString() ).c_str() )
            : QString("")
         );
         install = "";

         o2 = o->getAttr<Object*>("dl", &ok);
         if(!ok) continue;

         dls = o2->getAttr<Array*>(SYSTEM, &ok);
         if(!ok) continue;

         for(Value * vdl : *dls)
         {
            Object * dl = vdl->get<Object*>(&ok);
            if(!ok) continue;
            string way = dl->getAttr<string>("way", &ok);
            if(!ok) continue;
            if(way == "installer")
            {
               Array * urls = dl->getAttr<Array*>("urls", &ok);
               install += QString("<h3>%1</h3><p>").arg(tr("Installateurs :"));
               for(Value * url : *urls)
               {
                  string urlstr = url->get<string>(&ok);
                  QString str(urlstr.c_str());
                  QFileInfo info(str);
                  if(ok) install = install + "   <a href =\"" + str + "\">"+info.fileName()+"</a><br/>";
               }
               install += "</p>";
            }
         }



         Version version(QString(id.c_str()), desc, install);
         if(max<version) max = version;
      }
      FAIL:
      delete root;
   }



   END:
   if(!--reqRemaining && max != Version::self)
   {
      notifUpdate(max);
   }
}

void MainWindow::notifUpdate(const Version &v)
{
   QLabel * txt = new QLabel();
   txt->setTextFormat(Qt::RichText);
   txt->setOpenExternalLinks(true);
   txt->setText(QString("<h1>%1</h1> <h2>%2</h2><p>%3</p> <h2>%4</h2><p>%5</p>")
      .arg(tr("Mise à jour disponible : version %1").arg(QString("v%1.%2.%3.%4").arg(v.n[0]).arg(v.n[1]).arg(v.n[2]).arg(v.n[3])))
      .arg(tr("Liens :"))
      .arg(v.install)
      .arg(tr("Description :"))
      .arg(v.desc));
   displayWidget(tr("Une mise à jour est disponible"), txt, this);
}


