#ifndef WINDOWOFUSINGAUTO_H
#define WINDOWOFUSINGAUTO_H

#include <QDialog>

namespace Ui {
class WindowOfUsingAuto;
}



public:
    explicit WindowOfUsingAuto(QWidget *parent = 0);
    ~WindowOfUsingAuto();

private:
    Ui::WindowOfUsingAuto *ui;
};

#endif // WINDOWOFUSINGAUTO_H
