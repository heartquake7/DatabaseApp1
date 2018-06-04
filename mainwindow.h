#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include "ui_windowofadd.h"
#include "ui_mainwindow.h"
#include "ui_information.h"
#include "ui_windowofvehicle.h"
#include "ui_addin_vehicle_hijacking.h"
#include "ui_almostway.h"
#include "ui_tablevehicle.h"
#include "ui_windowofusingauto.h"
#include <QFile>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_triggered();
    void on_action_6_triggered();

    void on_action_9_triggered();


    void on_action_2_triggered();

    void on_action_3_triggered();

    void on_action_4_triggered();

    void on_action_7_triggered();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QSqlTableModel* nmodel;
    QSqlDatabase nDatabase;
    QSqlQueryModel              *modelMain;
    QSqlQueryModel              *modelDevice;

   // void createUI();
    void setupMainModel(const QStringList &headers);
};


class WindowOfAdd : public QDialog,public Ui::WindowOfAdd
{
    Q_OBJECT
private slots:
void on_pushButton_clicked();

void on_comboBox_activated(int arg1);
void on_comboBox_2_activated(int arg1);
void on_pushButton_2_clicked();

void on_pushButton_3_clicked();


private:
void WriteCash();
void ScanCash();
};
class Information : public QDialog,public Ui::Information
{
    Q_OBJECT

private slots:
    void on_pushButton_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);


};

class addin_vehicle_hijacking: public QDialog,public Ui::addin_vehicle_hijacking
{
    Q_OBJECT

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

private:
  QSqlTableModel* nmodel1;

};
class WindowOfUsingAuto : public QDialog,public Ui::WindowOfUsingAuto
{
    Q_OBJECT

private slots:
    void on_pushButton_clicked();
    void on_WindowOfUsingAuto_accepted();
};
class windowOfVehicle : public QDialog,public Ui::windowOfVehicle
{
    Q_OBJECT

private slots:
    void on_pushButton_clicked();
};
class almostway : public QDialog,public Ui::almostway
{
    Q_OBJECT

private slots:
    void on_pushButton_clicked();
};
class tablevehicle : public QDialog,public Ui::tablevehicle
{
    Q_OBJECT

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

private:
    QSqlTableModel* nmodel2;
};
#endif // MAINWINDOW_H
