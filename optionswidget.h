#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include "ui_optionswidget.h"

class OptionsWidget : public QDialog, private Ui::OptionsWidget
{
   Q_OBJECT
   
public:
   explicit OptionsWidget(QWidget *parent = 0);
};

#endif // OPTIONSWIDGET_H
