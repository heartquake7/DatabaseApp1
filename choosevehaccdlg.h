#ifndef CHOOSEVEHACCDLG_H
#define CHOOSEVEHACCDLG_H

#include <QDialog>
#include <QSqlRelation>
#include "database.h"

namespace Ui {
class chooseVehAccDlg;
}

class chooseVehAccDlg : public QDialog
{
    Q_OBJECT

public:
    explicit chooseVehAccDlg(QWidget *parent = 0);
    ~chooseVehAccDlg();
    void setupModel(const QStringList &headers);
    void createUI();

signals:
    void sendData(int);

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_btn_search_clicked();

    void on_btn_reset_clicked();

private:
    Ui::chooseVehAccDlg *ui;
    QSqlQueryModel* model;
};

#endif // CHOOSEVEHDLG_H
