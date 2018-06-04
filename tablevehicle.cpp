#include "tablevehicle.h"
#include "ui_tablevehicle.h"

tablevehicle::tablevehicle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tablevehicle)
{
    ui->setupUi(this);
}

tablevehicle::~tablevehicle()
{
    delete ui;
}
