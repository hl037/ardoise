#ifndef OPTIONS_H
#define OPTIONS_H

#include <QHash>
#include <QObject>
#include <QTranslator>
#include <QVariant>
#include <QStringList>
#include <json/ljsonp.hpp>

// TODO ! Faire un système permettant de construire le widget des options

class OptionsWidget;
class QLabel;

class Option : public QObject
{
   Q_OBJECT
protected:
   QString m_name;
   QString m_path;
   const char * m_text;
   const char * desc;
   QVariant value;

public:
   Option(const QString & m_name, QVariant value, const char * m_text, const char * desc = "", const QString & m_path = "general");
   inline QMetaType type(){return value.type();}
   virtual QVariant getValue() { return value; }
   virtual bool setValue(const QVariant & value);
   virtual QWidget * modifier();//Transpher ownership!!
   virtual QLabel * label();//Transpher ownership!!
   virtual QString text();//Transpher ownership!!
   virtual bool applyModifier(QWidget * w);

   inline QString name(){return m_name;}
   inline QString path(){return m_path;}
};

///////////////////////////////////////////////////////////////////////////////

class Options : public QObject
{
   Q_OBJECT
protected:

   Options();
   static QHash<QString, Option*> optionList;

   static Options * options;

public:
   static void create();
   static void destroy();
   static Options * instance();
   static OptionsWidget * optionsWidget();//Transpher ownership!!

   static bool addOption(Option* opt);
   static QVariant get(const QString & key);
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
   ComboOption(const QString & m_name, int defaultValue, const QStringList & choices, const char * m_text, const char * desc = "", const QString & m_path = "general");

   virtual bool setValue(const QVariant & value);
   virtual bool setValue(const QString &str);
   virtual bool setValue(int ind);
   virtual QWidget * modifier();//Transpher ownership!!
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
   ComboOption(const QString & m_name, int defaultValue, const QList<char*> & choices, const char * m_text, const char * desc = "", const QString & m_path = "general");
   virtual QWidget * modifier();//Transpher ownership!!
};

///////////////////////////////////////////////////////////////////////////////

class LanguageOption : public ComboOption
{
   Q_OBJECT
protected:
   QStringList languages;

public:
   LanguageOption(const QString & m_name, int defaultValue, const QStringList & dispLangugages, const QStringList languages, const char * m_text, const char * desc = "", const QString & m_path = "general");
   virtual bool setValue(int ind);
};

#endif // OPTIONS_H
