#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QDialog>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "database.h"
#include "regstartwnd.h"
#include "accstartwnd.h"
#include "test.h"


namespace Ui {
class StartWindow;
}

class StartWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = 0, database* = 0);
    ~StartWindow();

private slots:


    void on_btn_guest_clicked();

    void on_btn_login_clicked();

    void on_btn_submit_clicked();

    void on_pushButton1_clicked();

    void on_btn_back_clicked();

    void on_pushButton2_clicked();

private:
    Ui::StartWindow *ui;
    database* db;
};

#endif // STARTWINDOW_H
