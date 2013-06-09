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
   };

   Node root;
   
public:
   explicit OptionsWidget(const initializer_list & li);
   void addOption(Option *opt);
};

#endif // OPTIONSWIDGET_H
