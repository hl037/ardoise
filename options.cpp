#include "options.h"
#include <QCoreApplication>
#include <QCheckBox>
#include <QSpinBox>
#include <QFormLayout>
#include <QComboBox>


Option::Option(const QString &_name, QVariant _value, char * _text, char * _desc, const QString &_path) :
QObject(),
name(_name),
path(_path),
text(_text),
desc(_desc),
value(_value)
{
}

bool Option::setValue(const QVariant &_value)
{
   if(_value.type()!=value.type()) return false;
   value = _value;
   return true;
}

QWidget *Option::modWithLab(QWidget *mod)
{
   QWidget * w;

}

QWidget *Option::modifier()
{
   QWidget * w;
   switch(value.Type)
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
   }
   w->setToolTip(tr(desc));
   return w;
}

QLabel *Option::label()
{
   return new QLabel(tr(text));
}

static Options * Options::options = nullptr;

///////////////////////////////////////////////////////////////////////////////

Options::Options(int ind) :
QObject()
{
   changeLanguage(ind);
   QCoreApplication * app = QCoreApplication::instance();
   app->installTranslator(translator);
}

Options::Options(const QString &locale) :
QObject()
{
   changeLanguage(locale);
   QCoreApplication * app = QCoreApplication::instance();
   app->installTranslator(&translator);
}

void Options::create(int ind)
{
   if(options) throw "two instances of singleton Options";
   options = new Options(ind);
}

void Options::destroy()
{
   delete options;
   options = nullptr;
}

Options *Options::instance()
{
   if(!options) throw "no instance of Options";
   return options;;
}

///////////////////////////////////////////////////////////////////////////////

ComboOption::ComboOption(const QString &_name, int defaultValue, const QStringList &_choices, const char *_text, const char *_desc, const QString &_path) :
Option(_name, defaultValue, _text, _desc, _path),
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
      return setValue(_value.toInt())
   }
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

QWidget *ComboOption::modifier()
{
   QComboBox * cb = new QComboBox();
   cb->addItems(choices);
   return cb;
}

void ComboOption::changeValue(int ind)
{
   setValue(ind);
}

///////////////////////////////////////////////////////////////////////////////

QStringList ComboOptionTr::csl2sl(const QList<char *> &l1)
{
   QStringList l2;
   for(char * entry : choices)
   {
      l2<<QString(entry);
   }
   return l2;
}

ComboOptionTr::ComboOption(const QString &_name, int _defaultValue, const QList<char *> &_choices, const char *_text, const char *_desc, const QString &_path) :
ComboOption(_name, _defaultValue, csl2sl(_choices), _text, _desc, _path)
{
}

QWidget *ComboOptionTr::modifier()
{
   QStringList ch2;
   for(char * entry : choices)
   {
      ch2<<tr(entry);
   }
   QComboBox * cb = new QComboBox();
   cb->addItems(choices);
   return cb;
}

///////////////////////////////////////////////////////////////////////////////

extern QTranslator * translator;

LanguageOption::LanguageOption(const QString &_name, int _defaultValue, const QStringList & dispLangugages, const QStringList _languages, const char *_text, const char *_desc, const QString &_path) :
ComboOption(_name, _defaultValue, dispLangugages, _text, _desc, _path),
languages(_languages)
{
}

bool LanguageOption::setValue(int ind)
{
   if(!ComboOption::setValue(ind)) return false;
   translator->load("ardoise", ":/translations/", QString(), languages[ind]);
   return true;
}
