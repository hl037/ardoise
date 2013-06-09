#include "options.h"
#include <QCoreApplication>
#include <QCheckBox>
#include <QSpinBox>
#include <QFormLayout>
#include <QComboBox>

#include "json/ljsonp.hpp"


Option::Option(const QString &_name, QVariant _value, char * _text, char * _desc, const QString &_path) :
QObject(),
m_name(_name),
m_path(_path),
m_text(_text),
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
   default: ;
   }
   w->setToolTip(tr(desc));
   return w;
}

QLabel *Option::label()
{
   if(!*m_text) return null;
   return new QLabel(text());
}

QString Option::text()
{
   return tr(m_text);
}

bool Option::applyModifier(QWidget *w)
{
   switch(value.Type)
   {
   case QMetaType::Bool:
   {
      QCheckBox * cb = static_cast<QCheckBox*>(w);
      return setValue(QVariant(cb->isChecked()));
   }
   case QMetaType::Int:
   {
      QSpinBox * sb = static_cast<QSpinBox*(w);
      return setValue(sb->value());
   }
   default :
      return false;
   }
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

void Options::create()
{
   if(options) throw "two instances of singleton Options";
   options = new Options();
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

OptionsWidget *Options::optionsWidget()
{
}

bool Options::addOption(Option *opt)
{
   auto it = optionList.find(opt->name());
   if(it == optionList.end())
   {
      optonList[opt->name()] = opt;
   }
   else
   {
      return it.value() == opt ? true : (delete opt, false);
   }
   return true;
}

QVariant Options::get(const QString &key)
{
   auto it = optionList.find(key);
   if(it == optionList.end()) return QVariant();
   return it.value()->getValue();
}

bool Options::set(const QString &key, QVariant value)
{
   auto it = optionList.find(key);
   if(it == optionList.end())
   {
      Option * opt = new Option(key, value, "");
      optionList[key] = opt;
      return true;
   }
   else
   {
      return it.value()->setValue(value);
   }
}

static inline QVariant value2variant(ljsoncpp::Value * v)
{
   using namespace std;
   using namespace ljsoncpp;
   switch(v->type())
   {
   case NUMBER:
   {
      long double val = v->get<long double>();
      int val2 = val;
      if(val == (long double) val2)
      {
         return QVariant(val2);
      }
      else return QVariant(val);
   }
   case STRING:
      return QVariant(QString(v->get<string>().c_str()));
   case BOOL:
      return QVariant(v->get<bool>());
   default:
      return QVariant();
   }
}

void Options::readConf(std::istream &in)
{
   using namespace std;
   using namespace ljsoncpp;
   bool ok;
   Value * v = Parser::parse(in);
   if(!v) goto FAIL;
   Object * root = v->get<Object*>(&ok);
   if(!ok) goto FAIL;
   for(Object::iterator it : *root)
   {
      set(QString(it->first.c_str()), value2variant(it->second));
   }
}

void Options::saveConf(std::ostream &out)
{
   //TODO need json writter
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

bool ComboOption::applyModifier(QWidget *w)
{
   QComboBox * cb = qobject_cast<QComboBox *>(w);
   if(!cb) return false;
   return setValue(cb->currentIndex());
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
