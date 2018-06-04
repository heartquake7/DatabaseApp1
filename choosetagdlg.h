#ifndef CHOOSETAGDLG_H
#define CHOOSETAGDLG_H

#include <QDialog>
#include <QSqlRelation>
#include "database.h"

namespace Ui {
class chooseTagDlg;
}

class chooseTagDlg : public QDialog
{
    Q_OBJECT

public:
    explicit chooseTagDlg(QWidget *parent = 0);
    ~chooseTagDlg();
    void setupModel(const QStringList &headers);
    void createUI();

signals:
    void sendData(int);

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::chooseTagDlg *ui;
    QSqlQueryModel* model;
};

#endif // CHOOSETAGDLG_H
