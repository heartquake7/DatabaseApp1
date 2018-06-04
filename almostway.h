#ifndef ALMOSTWAY_H
#define ALMOSTWAY_H

#include <QDialog>

namespace Ui {
class almostway;
}

class almostway : public QDialog
{
    Q_OBJECT

public:
    explicit almostway(QWidget *parent = 0);
    ~almostway();

private:
    Ui::almostway *ui;
};

#endif // ALMOSTWAY_H
