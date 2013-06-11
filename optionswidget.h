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

#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include "ui_optionswidget.h"
#include <initializer_list>
#include <QHash>

class Option;

class QLabel;

class OptionsWidget : public QDialog, private Ui::OptionsWidget
{
   Q_OBJECT

protected:
   struct FormRow
   {
      QLabel * l;
      QWidget * w;
      Option * opt;
   };
   typedef QList<FormRow> FormRowList;

   class Node;
   typedef QHash<QString, Node*> NodeTable;

   struct Node
   {
      NodeTable childs;
      FormRowList rows;
      QWidget * w;
      int ind;
      inline Node():w(0){}
   };

   Node * root;

   void build(Node *n, QWidget *w);
   void apply(Node *n);
   void cancel(Node *n);
   void restoreDefaults(Node *n);
   void retranslate(Node *n);
   inline void retranslate(){retranslate(root);}
   
   void changeEvent(QEvent * e);
public:
   explicit OptionsWidget(const std::initializer_list<Option*> & li);
   explicit OptionsWidget(const QList<Option*> & li);
   ~OptionsWidget();
   void addOption(Option *opt);

public slots:
   void build();
   void buttonClicked(QAbstractButton *b);
   void apply();
   void validate();
   void cancel();
   void restoreDefaults();
};

#endif // OPTIONSWIDGET_H
