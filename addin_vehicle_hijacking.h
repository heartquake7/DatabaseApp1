#ifndef ADDIN_VEHICLE_HIJACKING_H
#define ADDIN_VEHICLE_HIJACKING_H

#include <QDialog>

namespace Ui {
class addin_vehicle_hijacking;
}

class addin_vehicle_hijacking : public QDialog
{
    Q_OBJECT

public:
    explicit addin_vehicle_hijacking(QWidget *parent = 0);
    ~addin_vehicle_hijacking();

private:
    Ui::addin_vehicle_hijacking *ui;
};

#endif // ADDIN_VEHICLE_HIJACKING_H
