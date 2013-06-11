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

#include "optionswidget.h"
#include "options.h"
#include <QFormLayout>
#include <QLabel>

OptionsWidget::OptionsWidget(const std::initializer_list<Option *> &li) :
QDialog(nullptr),
root(new Node)
{
   setupUi(this);
   for(Option* opt : li)
   {
      addOption(opt);
   }
   build();
}

OptionsWidget::OptionsWidget(const QList<Option *> &li) :
QDialog(nullptr),
root(new Node)
{
   setupUi(this);
   for(Option* opt : li)
   {
      addOption(opt);
   }
   build();
}

OptionsWidget::~OptionsWidget()
{
   delete root;
}

void OptionsWidget::changeEvent(QEvent *e)
{
   QDialog::changeEvent(e);
   switch (e->type()) {
   case QEvent::LanguageChange:
      retranslateUi(this);
      retranslate();
      break;
   default:
      break;
   }
}

void OptionsWidget::addOption(Option *opt)
{
   FormRow r;
   r.l = opt->label();
   if(!r.l) return;
   r.opt = opt;
   r.w = opt->modifier();

   r.opt->storeCurrent();
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

void OptionsWidget::build(Node * n, QWidget * w)
{
   for(auto it = n->childs.begin(), end = n->childs.end() ; it!=end ; ++it)
   {
      it.value()->w = new QWidget();
      it.value()->ind = tabWidget->addTab(it.value()->w, tr(it.key().toLatin1().data()));
      build(it.value(), it.value()->w);
   }
   if(!n->rows.empty())
   {
      QFormLayout * fl = new QFormLayout();
      for(FormRow f : n->rows)
      {
         fl->addRow(f.l, f.w);
      }
      if(w->layout()) delete w->layout();
      w->setLayout(fl);
   }
}

void OptionsWidget::apply(OptionsWidget::Node *n)
{
   for(FormRow f : n->rows)
   {
      f.opt->applyModifier(f.w);
   }
   for(auto it = n->childs.begin(), end = n->childs.end() ; it!=end ; ++it)
   {
      apply(it.value());
   }
}

void OptionsWidget::cancel(OptionsWidget::Node *n)
{
   for(FormRow f : n->rows)
   {
      f.opt->restoreStored(f.w);
   }
   for(auto it = n->childs.begin(), end = n->childs.end() ; it!=end ; ++it)
   {
      cancel(it.value());
   }
}

void OptionsWidget::restoreDefaults(OptionsWidget::Node *n)
{
   for(FormRow f : n->rows)
   {
      f.opt->restoreDefault(f.w);
   }
   for(auto it = n->childs.begin(), end = n->childs.end() ; it!=end ; ++it)
   {
      restoreDefaults(it.value());
   }
}

void OptionsWidget::retranslate(OptionsWidget::Node *n)
{
   for(FormRow f : n->rows)
   {
      f.opt->retranslate(f.l, f.w);
   }
   for(auto it = n->childs.begin(), end = n->childs.end() ; it!=end ; ++it)
   {
      Node * n2 = it.value();
      tabWidget->setTabText(n2->ind, tr(it.key().toLatin1().data()));
      retranslate(n2);
   }
}


void OptionsWidget::build()
{
   build(root, tabWidget);
}

void OptionsWidget::buttonClicked(QAbstractButton* b)
{
   switch(buttonBox->buttonRole(b))
   {
   case QDialogButtonBox::ApplyRole:
      apply();
      break;
   case QDialogButtonBox::ResetRole:
      restoreDefaults();
      break;
   default:
      break;
   }
}

void OptionsWidget::apply()
{
   apply(root);
}

void OptionsWidget::validate()
{
   apply();
   accept();
}

void OptionsWidget::cancel()
{
   cancel(root);
   reject();
}

void OptionsWidget::restoreDefaults()
{
   restoreDefaults(root);
}
