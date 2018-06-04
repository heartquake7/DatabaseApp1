#ifndef ACCSTARTWND_H
#define ACCSTARTWND_H

#include <QWidget>
#include <QWidget>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QSqlRelation>
#include <QSqlRecord>
#include <QMenu>
#include <QSqlDriver>
#include "database.h"
#include "addaccdialog.h"
#include "webmapdialog.h"
#include "infodlg.h"

namespace Ui {
class AccStartWnd;
}

class AccStartWnd : public QWidget
{
    Q_OBJECT

public:
    explicit AccStartWnd(QWidget *parent = 0, database* = 0, access = guest);
    ~AccStartWnd();
    void setupModel(const QStringList &headers);
    void createUI();

private slots:
    void on_btn_back_clicked();
    void slotgetInfo();
    void slotDeleteRecord();
    void oncloseDlg();
    void slotCustomMenuReq(QPoint pos);

    void on_btn_map_clicked();

    void on_btn_showfilter_clicked();

    void on_btn_applyfilter_clicked();

    void on_btn_resetfilter_clicked();

    void on_btn_add_clicked();

private:
    Ui::AccStartWnd *ui;
    database* db;
    QSqlQuery query;
    QSqlQueryModel* model;
    QWidget* Parent;
    QStringList list;
    access Acc;
};

#endif // ACCSTARTWND_H
