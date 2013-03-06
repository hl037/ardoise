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

#include <QtGui>

#include "flowlayout.h"
#include <limits.h>

FlowLayout::FlowLayout(QWidget *parent, Qt::Orientation o, int hs, int vs, Qt::LayoutDirection d, bool e) :
QLayout(parent), m_o(o), m_hs(hs), m_vs(vs), m_d(d), m_e(e)
{

}

FlowLayout::FlowLayout(Qt::Orientation o, int hs, int vs, Qt::LayoutDirection d, bool e) :
QLayout(), m_o(o), m_hs(hs), m_vs(vs), m_d(d), m_e(e)
{

}

FlowLayout::~FlowLayout()
{
   QLayoutItem *item;
   while ((item = takeAt(0))) delete item;
}

void FlowLayout::addItem(QLayoutItem *it)
{
   items.append(it);
}

QLayoutItem * FlowLayout::itemAt(int i) const
{
   return i<items.length() ? items.at(i) : 0;
}

QLayoutItem * FlowLayout::takeAt(int i)
{
   return i<items.length() ? items.takeAt(i) : 0;
}

int FlowLayout::count() const
{
   return items.size();
}

void FlowLayout::setGeometry(const QRect &r)
{
   doLayout(r, true);
}

QSize FlowLayout::sizeHint() const
{
   int w=0,h=0;
   QSize s;
   auto it = items.begin();
   auto end = items.end();
   for(;it!=end ; ++it)
   {
      s = (*it)->sizeHint();
      w += s.width();
      if(h<s.height()) h = s.height();
   }
   s.setWidth(w);
   s.setHeight(h);
   return s;
}

bool FlowLayout::hasHeightForWidth() const
{
   return true;
}

int FlowLayout::heightForWidth(int w) const
{
   return doLayout(QRect(0,0,w,0), false).height();
}

QSize FlowLayout::doLayout(QRect rect, bool apply) const
{
   //Configuration des marges
   int x,y=0; //Coordonnées courrantes
   int left, right, top, bottom; //Marge du layout
   int h=0; //Hauteur de la ligne courante
   int w;//Largeur du layout
   int nbe;//Nombre de widget expensible latéralement
   int p;//nom de pixel à rajouter aux widget expendable
   QSize s;

   auto it_b = items.begin(), end = items.end();
   auto it_e = it_b;

   getContentsMargins(&left, & top, &right, &bottom);

   if(m_d == Qt::LeftToRight)
   {
      w = rect.width() - right;
      y = rect.top() + top;
      left += rect.left();
      while(it_b != end)
      {
         nbe = (*it_b)->expandingDirections() & Qt::Horizontal;
         x = left;
         y += h;
         s = (*it_b)->sizeHint();
         x += s.width();
         h = s.height();
         it_e = it_b;
         while(x < w && (++it_e) != end)
         {
            nbe += (*it_e)->expandingDirections() & Qt::Horizontal;
            s = (*it_e)->sizeHint();
            x += s.width() + m_hs;
            if(h<s.height()) h = s.height();
         }
         if(it_e == it_b) ++it_e;
         if(it_e != end)
         {
            x -= s.width();
            nbe -= (*it_b)->expandingDirections() & Qt::Horizontal;
         }

         if(nbe)
         {
            p = (w-x) / nbe;
            nbe = (w-x) % nbe;
         }
         else p=0;

         if(apply)
         {
            x = left;
            while(it_b != it_e)
            {
               s = (*it_b)->sizeHint();
               s.setHeight(h);
               if(m_e && ((*it_b)->expandingDirections() & Qt::Horizontal))
               {
                  if(nbe)
                  {
                     --nbe;
                     s.rwidth() += p+1;
                  }
                  else s.rwidth() += p;
               }
               (*it_b)->setGeometry(QRect(QPoint(x,y),s));
               ++it_b;
               x += s.width() + m_hs;
            }
         }
         it_b = it_e;
         y += m_vs;
      }
   }
   s.setWidth(rect.width());
   s.setHeight(y+h+bottom);
   return s;
}


Qt::LayoutDirection FlowLayout::layoutDirection() const
{
   return m_d;
}


void FlowLayout::setExpandsWidgets(bool e)
{
   m_e = e;
}

bool FlowLayout::expandsWidgets() const
{
   return m_e;
}



void FlowLayout::setHSpace(int hs)
{
   m_hs = hs;
}

int FlowLayout::HSpace() const
{
   return m_hs;
}



void FlowLayout::setVSpace(int vs)
{
   m_vs = vs;
}

int FlowLayout::VSpace() const
{
   return m_vs;
}
