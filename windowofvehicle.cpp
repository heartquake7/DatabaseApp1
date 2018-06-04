#include "windowofvehicle.h"
#include "ui_windowofvehicle.h"

windowOfVehicle::windowOfVehicle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::windowOfVehicle)
{
    ui->setupUi(this);
}

windowOfVehicle::~windowOfVehicle()
{
    delete ui;
}
