#include "almostway.h"
#include "ui_almostway.h"

almostway::almostway(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::almostway)
{
    ui->setupUi(this);
}

almostway::~almostway()
{
    delete ui;
}
