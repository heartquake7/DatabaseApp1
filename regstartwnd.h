#ifndef REGSTARTWND_H
#define REGSTARTWND_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QSqlRelation>
#include <QSqlRecord>
#include <QMenu>
#include "database.h"
#include "addregdialog.h"

namespace Ui {
class RegStartWnd;
}

class RegStartWnd : public QWidget
{
    Q_OBJECT

public:
    explicit RegStartWnd(QWidget *parent = 0, database* = 0, access = guest);
    ~RegStartWnd();
    void setupModel(const QStringList &headers);
    void createUI();

private slots:
    void on_btn_back_clicked();
    void slotEditRecord();
    void slotDeleteRecord();
    void oncloseDlg();
    void slotCustomMenuReq(QPoint pos);
    void on_btn_add_clicked();

    void on_btn_showsearch_clicked();

    void on_btn_search_clicked();

    void on_btn_reset_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_btn_showfilter_clicked();

    void on_btn_resetfilter_clicked();

    void on_btn_applyfilter_clicked();

private:
    Ui::RegStartWnd *ui;
    database* db;
    QSqlQuery query;
    QSqlQueryModel* model;
    QWidget* Parent;
    access Acc;
};

#endif // REGSTARTWND_H
