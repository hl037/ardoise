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
 *
 ********************************************/

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
