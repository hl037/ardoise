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
