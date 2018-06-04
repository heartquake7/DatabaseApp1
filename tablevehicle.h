#ifndef TABLEVEHICLE_H
#define TABLEVEHICLE_H

#include <QDialog>

namespace Ui {
class tablevehicle;
}

class tablevehicle : public QDialog
{
    Q_OBJECT

public:
    explicit tablevehicle(QWidget *parent = 0);
    ~tablevehicle();

private:
    Ui::tablevehicle *ui;
};

#endif // TABLEVEHICLE_H
