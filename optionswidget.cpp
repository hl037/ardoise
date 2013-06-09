#include "optionswidget.h"
#include "options.h"

OptionsWidget::OptionsWidget(const initializer_list &li) :
QDialog(nullptr)
{
   setupUi(this);
}

void OptionsWidget::addOption(Option *opt)
{
   FormRow r;
   r.l = lab = opt->label();
   if(!r.l) return;
   r.opt = opt;
   r.w = opt->modifier();
   QStringList path = opt->path().split(QChar('/'));
   Node * n = root;
   for(QString s : path)
   {
      auto it = n->childs.find(s);
      if(it == n->childs.end())
      {
         Node * n2 = new Node;
         it = n->childs.insert(s, n2);
      }
      n = it.value();
   }
   n->rows.append(r);
}
