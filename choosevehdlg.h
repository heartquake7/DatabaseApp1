#ifndef CHOOSEVEHDLG_H
#define CHOOSEVEHDLG_H

#include <QDialog>
#include <QSqlRelation>
#include "database.h"

namespace Ui {
class chooseVehDlg;
}

class chooseVehDlg : public QDialog
{
    Q_OBJECT

public:
    explicit chooseVehDlg(QWidget *parent = 0);
    ~chooseVehDlg();
    void setupModel(const QStringList &headers);
    void createUI();

signals:
    void sendData(int);

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::chooseVehDlg *ui;
    QSqlQueryModel* model;
};

#endif // CHOOSEVEHDLG_H
