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

#include <iostream>
#include "options.h"
#include <QCoreApplication>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QFormLayout>
#include <QComboBox>
#include <QLocale>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include "optionswidget.h"

Option::Option(const QString &name, const QVariant & _value, const char * text, const char * desc, const QString &path) :
QObject(),
m_name(name),
m_path(path),
m_text(text),
m_desc(desc),
value(_value),
storedValue(_value),
defaultValue(_value)
{
}

bool Option::setValue(const QVariant &_value)
{
   if(_value.type()!=value.type()) return false;
   value = _value;
   return true;
}

QWidget *Option::modifier() const
{
   QWidget * w = nullptr;
   switch(value.type())
   {
   case QMetaType::Bool:
   {
      QCheckBox * cb = new QCheckBox();
      cb->setChecked(value.toBool());
      w = cb;
      break;
   }
   case QMetaType::Int:
   {
      QSpinBox * sb = new QSpinBox();
      sb->setValue(value.toInt());
      w = sb;
      break;
   }
   default: ;
   }
   w->setToolTip(desc());
   return w;
}

void Option::retranslateModifier(QWidget *mod) const
{
   mod->setToolTip(desc());
}

QLabel *Option::label() const
{
   if(!*m_text) return nullptr;
   QLabel * lab = new QLabel(text());
   lab->setToolTip(desc());
   return lab;
}

void Option::retranslateLabel(QLabel *lab) const
{
   lab->setText(text());
   lab->setToolTip(desc());
}

QString Option::text() const
{
   return QCoreApplication::translate("OptionsWidget", m_text);
}

QString Option::desc() const
{
   return QCoreApplication::translate("OptionsWidget", m_desc);
}

bool Option::applyModifier(QWidget *w)
{
   switch(value.type())
   {
   case QMetaType::Bool:
   {
      QCheckBox * cb = static_cast<QCheckBox*>(w);
      return setValue(QVariant(cb->isChecked()));
   }
   case QMetaType::Int:
   {
      QSpinBox * sb = static_cast<QSpinBox*>(w);
      return setValue(sb->value());
   }
   default :
      break;
   }
   return false;
}

void Option::storeCurrent()
{
   storedValue = value;
}

void Option::restoreStored(QWidget *mod)
{
   restoreValue(mod, storedValue);
}

void Option::restoreDefault(QWidget *mod)
{
   restoreValue(mod, defaultValue);
}

void Option::restoreValue(QWidget *mod, const QVariant &v)
{
   switch(v.type())
   {
   case QMetaType::Bool:
   {
      QCheckBox * cb = static_cast<QCheckBox*>(mod);
      value = v;
      if(cb) cb->setChecked(value.toBool());
      break;
   }
   case QMetaType::Int:
   {
      QSpinBox * sb = static_cast<QSpinBox*>(mod);
      value = v;
      if(sb) sb->setValue(value.toInt());
      break;
   }
   default :
      break;
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool Options::m_created = false;
QHash<QString, Option*> Options::optionTable;
QList<Option*> Options::optionList;

void Options::create()
{
   if(m_created) throw "Options already created";
   m_created = true;
}

void Options::destroy()
{
   m_created = false;
   auto it = optionList.begin(), end = optionList.end();
   for(; it!=end; ++it)
   {
      delete *it;
   }
   optionTable.clear();
   optionList.clear();
}

OptionsWidget *Options::optionsWidget()
{
   return new OptionsWidget(optionList);
}

bool Options::addOption(Option *opt)
{
   auto it = optionTable.find(opt->name());
   if(it == optionTable.end())
   {
      optionTable[opt->name()] = opt;
      optionList.push_back(opt);
      return true;
   }
   if(it.value() == opt)
   {
      optionList.append(optionList.takeAt(optionList.indexOf(opt))); // NOTE : possibilité d'obtenir l'index en l'enregistrant dans Option
      return true;
   }
   delete opt;
   return false;
}

QVariant Options::get(const QString &key)
{
   auto it = optionTable.find(key);
   if(it == optionTable.end()) return QVariant();
   return it.value()->getValue();
}

const Option *Options::getOption(const QString &key)
{
   auto it = optionTable.find(key);
   if(it == optionTable.end()) return nullptr;
   return it.value();
}

bool Options::set(const QString &key, QVariant value)
{
   auto it = optionTable.find(key);
   if(it == optionTable.end())
   {
      Option * opt = new Option(key, value, "");
      optionTable[opt->name()] = opt;
      optionList.push_back(opt);
      return true;
   }
   else
   {
      return it.value()->setValue(value);
   }
}

static inline QVariant value2variant(const QJsonValue & v)
{
   switch(v.type())
   {
   case QJsonValue::Double :
   {
      double val = v.toDouble();
      int val2 = val;
      if(val == (double) val2)
      {
         return QVariant(val2);
      }
      else return QVariant((double) val);
   }
   case QJsonValue::String:
      return QVariant(v.toString());
   case QJsonValue::Bool:
      return QVariant(v.toBool());
   case QJsonValue::Array:
      return QVariant(v.toArray().toVariantList());
   case QJsonValue::Object:
      return QVariant(v.toObject().toVariantMap());
   default:
      break;
   }
   return QVariant();
}

void Options::readConf(const QByteArray & in)
{
   QJsonParseError e;
   QJsonDocument doc = QJsonDocument::fromJson(in, &e);
   if(e.error != QJsonParseError::NoError)
   {
      std::cerr<<"at "<<e.offset<<" : "<<e.errorString().toStdString();
      return;
   }
   QJsonObject root = doc.object();
   for(QJsonObject::Iterator it = root.begin(), end = root.end() ; it!=end ; ++it)
   {
      set(it.key(), value2variant(it.value()));
   }
}

QByteArray Options::saveConf()
{
   QJsonObject o;
   for(auto it = optionList.begin(), end = optionList.end() ; it != end ; ++it)
   {
      Option * & val = *it;
      o[val->name()] = QJsonValue::fromVariant(val->getPrintedValue());
   }
   return QJsonDocument(o).toJson();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ComboOption::ComboOption(const QString &name, int defaultValue, const QStringList &_choices, const char *text, const char *desc, const QString &path) :
Option(name, defaultValue, text, desc, path),
value(defaultValue),
choices(_choices)
{
}

bool ComboOption::setValue(const QVariant &_value)
{
   if(_value.type() == QMetaType::QString)
   {
      return setValue(_value.toString());
   }
   else if(_value.type() == QMetaType::Int)
   {
      return setValue(_value.toInt());
   }
   return false;
}

bool ComboOption::setValue(const QString & str)
{
   return setValue(choices.indexOf(str));
}

bool ComboOption::setValue(int ind)
{
   if(ind<0 || ind>=choices.size()) return false;
   value = ind;
   emit valueChanged(value);
   return true;
}

QWidget *ComboOption::modifier() const
{
   QComboBox * cb = new QComboBox();
   cb->addItems(choices);
   cb->setCurrentIndex(value);
   return cb;
}

bool ComboOption::applyModifier(QWidget *w)
{
   QComboBox * cb = qobject_cast<QComboBox *>(w);
   if(!cb) return false;
   return setValue(cb->currentIndex());
}

void ComboOption::storeCurrent()
{
   storedValue = value;
}

void ComboOption::restoreValue(QWidget *mod, const QVariant &v)
{
   QComboBox * cb = qobject_cast<QComboBox *>(mod);
   if(!cb) return;
   setValue(v.toInt());
   cb->setCurrentIndex(v.toInt());
}

void ComboOption::changeValue(int ind)
{
   setValue(ind);
}

///////////////////////////////////////////////////////////////////////////////

QStringList ComboOptionTr::csl2sl(const QList<char *> &l1)
{
   QStringList l2;
   for(char * entry : l1)
   {
      l2<<QString(entry);
   }
   return l2;
}

ComboOptionTr::ComboOptionTr(const QString & name, int _defaultValue, const QList<char *> & _choices, const char *_text, const char *_desc, const QString &_path) :
ComboOption(name, _defaultValue, csl2sl(_choices), _text, _desc, _path)
{
}

QWidget *ComboOptionTr::modifier() const
{
   QStringList ch2;
   for(char * entry : choices)
   {
      ch2<<QCoreApplication::translate("OptionsWidget", entry);
   }
   QComboBox * cb = new QComboBox();
   cb->addItems(ch2);
   return cb;
}

///////////////////////////////////////////////////////////////////////////////

extern QTranslator * translator;

static QStringList nativeLangNames(const QList<QLocale> &locales)
{
   QStringList l;
   for(auto it = locales.begin(), end = locales.end() ; it!=end ; ++it)
   {
      l<<it->nativeLanguageName();
   }
   return l;
}

LanguageOption::LanguageOption(const QString &name, int defaultValue, const QList<QLocale> &locales, const char *text, const char *desc, const QString &path) :
ComboOption(name, defaultValue, nativeLangNames(locales), text, desc, path),
m_locales(locales)
{
}

bool LanguageOption::setValue(const QString &str)
{
   QLocale l(str);
   int ind = m_locales.indexOf(l);
   if(ind != -1) return setValue(ind);
   return ComboOption::setValue(str);
}

bool LanguageOption::setValue(int ind)
{
   if(!ComboOption::setValue(ind)) return false;
   translator->load(m_locales[value], "ardoise",  "_", ":/translations/");
   return true;
}

QVariant LanguageOption::getPrintedValue() const
{
   return QVariant(currentLang());
}

QLocale LanguageOption::currentLocale() const
{
   return m_locales[value];
}

QString LanguageOption::currentLang() const
{
   return currentLocale().name().split("_").at(0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StringListOption::StringListOption(const QString &name, const QVariant &value, const char *text, const char *desc, const QString &path) :
Option(name, value, text, desc, path)
{
}

bool StringListOption::setValue(const QVariant &_value)
{
   if(_value.canConvert<QStringList>())
   {
      value = _value.toStringList();
      return true;
   }
   return false;
}
