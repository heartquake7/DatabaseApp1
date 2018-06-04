#ifndef INFODLG_H
#define INFODLG_H

#include <QDialog>
#include <QSqlQuery>

namespace Ui {
class InfoDlg;
}

class InfoDlg : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDlg(QWidget *parent = 0, int id = 0);
    ~InfoDlg();

private slots:
    void on_btn_ok_clicked();

private:
    void getinfo();
    QSqlQuery query;
    Ui::InfoDlg *ui;
    int ID;
};

#endif // INFODLG_H
