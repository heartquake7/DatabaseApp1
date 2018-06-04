#ifndef CHOOSEOWNDLG_H
#define CHOOSEOWNDLG_H

#include <QDialog>
#include <QSqlRelation>
#include "database.h"

namespace Ui {
class chooseOwnDlg;
}

class chooseOwnDlg : public QDialog
{
    Q_OBJECT

public:
    explicit chooseOwnDlg(QWidget *parent = 0);
    ~chooseOwnDlg();
    void setupModel(const QStringList &headers);
    void createUI();

signals:
    void sendData(int);

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_btn_search_clicked();

    void on_btn_reset_clicked();

private:
    Ui::chooseOwnDlg *ui;
    QSqlQueryModel* model;
};

#endif // CHOOSEOWNDLG_H
