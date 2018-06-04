#include "choosetagdlg.h"
#include "ui_choosetagdlg.h"

chooseTagDlg::chooseTagDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chooseTagDlg)
{
    ui->setupUi(this);
    this->setupModel(QStringList() << trUtf8("ID") << trUtf8("Гос. Номер"));
    this->createUI();
}

chooseTagDlg::~chooseTagDlg()
{
    delete ui;
}

void chooseTagDlg::setupModel(const QStringList &headers)
{
    model = new QSqlQueryModel(this);

    //Запрос на выборку свободных гос. номеров из базы
    model->setQuery("SELECT `tag_id`, `vehicle_registration_tag` FROM `vehicle_registration_tag` WHERE"
                    "`tag_id` NOT IN (SELECT `tag_id` FROM `vehicle_registration` WHERE `removal_date` IS NULL) ORDER BY `vehicle_registration_tag`");

    for (int i = 0, j = 0; i < model->columnCount(); i++, j++)
    {
        model->setHeaderData(i, Qt::Horizontal, headers[j]);
    }
}

//Определение параметров таблицы
void chooseTagDlg::createUI()
{
      ui->tableView->setModel(model);
      ui->tableView->setColumnHidden(0, true);
      ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
      ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
      ui->tableView->resizeColumnsToContents();
      ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
      ui->tableView->horizontalHeader()->setStretchLastSection(true);
      ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

//Обработка двойного клика на строке таблицы
void chooseTagDlg::on_tableView_doubleClicked(const QModelIndex &index)
{
    QModelIndex chosen = index;
    int id = chosen.sibling(chosen.row(), 0).data().toInt();
    emit sendData(id);
    this->close();
}
