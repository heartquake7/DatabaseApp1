#ifndef ADDACCDIALOG_H
#define ADDACCDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidgetItem>
#include "choosevehaccdlg.h"

namespace Ui {
class addAccDialog;
}

class addAccDialog : public QDialog
{
    Q_OBJECT

signals:
    void closethis();

public:
    explicit addAccDialog(QWidget *parent = 0);
    ~addAccDialog();

private slots:
    void on_btn_submit_clicked();

    void on_lineEdit_3_textEdited(const QString &arg1);

    void on_btn_choose_clicked();

    void receiveDataVeh(int);

    void on_btn_add_clicked();

    void on_btn_delete_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    int chosenVehId;
    QMap<QListWidgetItem*, QStringList> map;
    chooseVehAccDlg* chooseVeh;
    Ui::addAccDialog *ui;
    QList<QLineEdit *> m_list;
    bool checkfillacc();
    bool checkfillpart();
};

#endif // ADDACCDIALOG_H
