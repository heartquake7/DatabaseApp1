#include "windowofusingauto.h"
#include "ui_windowofusingauto.h"

WindowOfUsingAuto::WindowOfUsingAuto(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WindowOfUsingAuto)
{
    ui->setupUi(this);
}

WindowOfUsingAuto::~WindowOfUsingAuto()
{
    delete ui;
}
