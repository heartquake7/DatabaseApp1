#ifndef ADDREGDIALOG_H
#define ADDREGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTimer>
#include "database.h"
#include "choosevehdlg.h"
#include "chooseowndlg.h"
#include "choosetagdlg.h"

namespace Ui {
class addRegDialog;
}

class addRegDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addRegDialog(QWidget *parent = 0, database* = 0);
    ~addRegDialog();

signals:
    void closethis();

private slots:
    void on_comboBoxtype_currentIndexChanged(int index);

    void on_lineEdit_vin_textEdited(const QString &arg1);

    void on_btn_choosevehicle_clicked();

    void on_btn_chooseowner_clicked();

    void on_btn_submit_clicked();

    void on_comboBoxowner_currentIndexChanged(int index);

    void on_btn_choosetag_clicked();

    void receiveDataVeh(int);

    void receiveDataOwn(int);

    void receiveDataTag(int);

    void on_btn_cancelvehicle_clicked();

    void on_btn_cancelowner_clicked();

private:
    int chosenVehId;
    int chosenOwnId;
    int chosenTagId;
    chooseVehDlg* chooseVeh;
    chooseOwnDlg* chooseOwn;
    chooseTagDlg* chooseTag;
    Ui::addRegDialog *ui;
    database* db;
    QList<QLineEdit *> m_list;
    bool checkfill();
};

#endif // ADDREGDIALOG_H
