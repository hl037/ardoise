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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QHash>
#include <QObject>
#include <QTranslator>
#include <QVariant>
#include <QStringList>
#include "json/ljsonp.hpp"
#include <QLocale>

// TODO ! Faire un système permettant de construire le widget des options

class OptionsWidget;
class QLabel;
class Options;

class Option : public QObject
{
   Q_OBJECT
protected:

   friend class Options;

   QString m_name;
   QString m_path;
   const char * m_text;
   const char * m_desc;
   QVariant value;

public:
   Option(const QString & name, const QVariant & value, const char * text, const char * desc = "", const QString & path = "general");
   inline QMetaType::Type type() const {return static_cast<QMetaType::Type>(value.type());}
   virtual QVariant getValue() const { return value; }
   virtual bool setValue(const QVariant & value);
   virtual QWidget * modifier() const;//Transpher ownership!!
   virtual void retranslateModifier(QWidget * mod) const;
   virtual QLabel * label() const;//Transpher ownership!!
   virtual void retranslateLabel(QLabel * lab) const;
   inline void retranslate(QLabel * lab, QWidget * mod) const {retranslateLabel(lab); retranslateModifier(mod);}
   virtual QString text() const;
   virtual QString desc() const;
   virtual bool applyModifier(QWidget * w);

   inline QString name() const {return m_name;}
   inline QString path() const {return m_path;}
};

///////////////////////////////////////////////////////////////////////////////

class Options : public QObject
{
   Q_OBJECT
protected:
   static QHash<QString, Option*> optionTable;
   static QList<Option *> optionList;//Pour garder l'ordre d'insertion
   static bool m_created;

public:
   static inline bool created() { return m_created; }
   static void create();
   static void destroy();
   static OptionsWidget * optionsWidget();//Transpher ownership!!

   static bool addOption(Option* opt);
   static QVariant get(const QString & key);
   static const Option* getOption(const QString & key);
   static bool set(const QString & key, QVariant value);

   static void readConf(std::istream & in);
   static void saveConf(std::ostream & out);
};

///////////////////////////////////////////////////////////////////////////////

class ComboOption : public Option
{
   Q_OBJECT
protected:
   int value;
   QStringList choices;

public:
   ComboOption(const QString & name, int defaultValue, const QStringList & choices, const char * text, const char * desc = "", const QString & path = "general");

   virtual bool setValue(const QVariant & value);
   virtual bool setValue(const QString &str);
   virtual bool setValue(int ind);
   virtual QWidget * modifier() const;//Transpher ownership!!
   virtual bool applyModifier(QWidget * w);

public slots:
   void changeValue(int ind);

signals:
   void valueChanged(int ind);
};

///////////////////////////////////////////////////////////////////////////////

class ComboOptionTr : public ComboOption
{
   Q_OBJECT
protected:
   QList<char*> choices;
   static QStringList csl2sl(const QList<char*> & l1);
public:
   ComboOptionTr(const QString & name, int defaultValue, const QList<char*> & choices, const char * text, const char * desc = "", const QString & path = "general");
   virtual QWidget * modifier() const;//Transpher ownership!!
};

///////////////////////////////////////////////////////////////////////////////

class LanguageOption : public ComboOption
{
   Q_OBJECT
protected:
   QList<QLocale> m_locales;

public:
   LanguageOption(const QString & name, int defaultValue, const QList<QLocale> & locales, const char * text, const char * desc = "", const QString & path = "general");
   virtual bool setValue(int ind);

   QLocale currentLocale() const;
};

#endif // OPTIONS_H
