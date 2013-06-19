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

#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QWidget>
#include <QLayout>
#include <QRect>
#include <QStyle>
#include <QWidgetItem>

class FlowLayout : public QLayout
{
public:
   FlowLayout(QWidget *parent, Qt::Orientation o = Qt::Horizontal, int hs = 0, int vs = 0, Qt::LayoutDirection d = Qt::LeftToRight, bool e=true);
   FlowLayout(Qt::Orientation o = Qt::Horizontal, int hs = 0, int vs = 0, Qt::LayoutDirection d = Qt::LeftToRight, bool e=true);
   ~FlowLayout();

   void addItem(QLayoutItem * it);
   QLayoutItem * itemAt(int i) const;
   QLayoutItem * takeAt(int i);
   int count() const;

   void setLayoutDirection(Qt::LayoutDirection d);
   Qt::LayoutDirection layoutDirection() const;

   void setGeometry(const QRect & r);
   QSize sizeHint() const;

   bool hasHeightForWidth() const;
   int heightForWidth(int w) const;

   void setExpandsWidgets(bool e);
   bool expandsWidgets() const;

   void setHSpace(int hs);
   int HSpace() const;

   void setVSpace(int vs);
   int VSpace() const;

protected:
   QSize doLayout(QRect rect, bool apply) const;

   QList<QLayoutItem*> items;

   Qt::Orientation m_o;
   int m_hs, m_vs;
   Qt::LayoutDirection m_d;
   bool m_e;
};

#endif
