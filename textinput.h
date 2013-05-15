#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <QLineEdit>

class TextInput : public QLineEdit
{
   Q_OBJECT
public:
   explicit TextInput(QWidget *parent = 0);

   int lineHeight();

protected:
   void keyPressEvent(QKeyEvent * e);
   
signals:
   void accepted();
   void rejected();
   void nextLine();
   
public slots:
   
};

#endif // TEXTINPUT_H
