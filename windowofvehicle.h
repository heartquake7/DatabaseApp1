#ifndef WINDOWOFVEHICLE_H
#define WINDOWOFVEHICLE_H

#include <QDialog>

namespace Ui {
class windowOfVehicle;
}

class windowOfVehicle : public QDialog
{
    Q_OBJECT

public:
    explicit windowOfVehicle(QWidget *parent = 0);
    ~windowOfVehicle();

private:
    Ui::windowOfVehicle *ui;
};

#endif // WINDOWOFVEHICLE_H
