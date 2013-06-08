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
   QString name;
   QString path;
   const char * text;
   const char * desc;
   QVariant value;

public:
   Option(const QString & name, QVariant value, const char * text, const char * desc = "", const QString & path = "general");
   inline QMetaType type(){return value.type();}
   virtual bool setValue(const QVariant & value);
   virtual QWidget * modifier();//Transpher ownership!!
   virtual QLabel * label();//Transpher ownership!!
};

///////////////////////////////////////////////////////////////////////////////

class Options : public QObject
{
   Q_OBJECT
protected:

   Options();

   static Options * options;

public:
   static void create();
   static void destroy();
   static Options * instance();
   static const char * const * const langs;
   static OptionsWidget * optionsWidget();//Transpher ownership!!
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
   virtual QWidget * modifier();//Transpher ownership!!

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
   ComboOption(const QString & name, int defaultValue, const QList<char*> & choices, const char * text, const char * desc = "", const QString & path = "general");
   virtual QWidget * modifier();//Transpher ownership!!
};

///////////////////////////////////////////////////////////////////////////////

class LanguageOption : public ComboOption
{
   Q_OBJECT
protected:
   QStringList languages;

public:
   LanguageOption(const QString & name, int defaultValue, const QStringList & dispLangugages, const QStringList languages, const char * text, const char * desc = "", const QString & path = "general");
   virtual bool setValue(int ind);
};

#endif // OPTIONS_H
