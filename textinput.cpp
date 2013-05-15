#include "textinput.h"
#include <QKeyEvent>

TextInput::TextInput(QWidget *parent) :
QLineEdit(parent)
{
}

int TextInput::lineHeight()
{
   int s = font().pixelSize();
   //NOTE : La distance entre deux lignes pourrait devenir une option
   return s * 1.3 +1;
}

void TextInput::keyPressEvent(QKeyEvent *e)
{
   if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return )
   {
      if(e->modifiers() & Qt::ShiftModifier)
      {
         emit nextLine();
      }
      else
      {
         emit accepted();
      }
      e->accept();
   }
   else if(e->key() == Qt::Key_Escape)
   {
      emit rejected();
      e->accept();
   }
   else
   {
      QLineEdit::keyPressEvent(e);
   }
}
