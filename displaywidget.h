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
