#include "addin_vehicle_hijacking.h"
#include "ui_addin_vehicle_hijacking.h"

addin_vehicle_hijacking::addin_vehicle_hijacking(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addin_vehicle_hijacking)
{
    ui->setupUi(this);
}

addin_vehicle_hijacking::~addin_vehicle_hijacking()
{
    delete ui;
}
