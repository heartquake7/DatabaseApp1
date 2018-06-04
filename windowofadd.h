#ifndef WINDOWOFADD_H
#define WINDOWOFADD_H

#include <QDialog>

namespace Ui {
class WindowOfAdd;
}

class WindowOfAdd : public QDialog
{
    Q_OBJECT

public:
    explicit WindowOfAdd(QWidget *parent = 0);
    ~WindowOfAdd();




private:
    Ui::WindowOfAdd *ui;
};

#endif // WINDOWOFADD_H
