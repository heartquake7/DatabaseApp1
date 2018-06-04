#include "choosevehaccdlg.h"
#include "ui_choosevehaccdlg.h"

chooseVehAccDlg::chooseVehAccDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chooseVehAccDlg)
{
    ui->setupUi(this);
    this->setupModel(QStringList() << trUtf8("ID") << trUtf8("Транспортное средство") << trUtf8("Гос. номер")
                     << trUtf8("VIN-код"));
    this->createUI();
}

chooseVehAccDlg::~chooseVehAccDlg()
{
    delete ui;
}

void chooseVehAccDlg::setupModel(const QStringList &headers)
{
    model = new QSqlQueryModel(this);

    model->setQuery("SELECT `vehicle_id`, `brand_name`, `vehicle_registration_tag`, `vin_code` FROM (SELECT `vehicle_id`, "
                    "`brand_name`, `t`.`vehicle_registration_tag`, `vin_code`, `removal_date` "
                    "FROM (SELECT `v`.`vehicle_id`, `brand_name`, `vin_code`, `tag_id`, `removal_date` FROM `vehicle` "
                    "as `v` INNER JOIN `vehicle_registration` as `r` ON `v`.`vehicle_id` = `r`.`vehicle_id`) "
                    "as `one` INNER JOIN `vehicle_registration_tag` as `t` ON `t`.`tag_id` = `one`.`tag_id`) as `res` "
                    "WHERE `removal_date` IS NULL");

    for (int i = 0, j = 0; i < model->columnCount(); i++, j++)
    {
        model->setHeaderData(i, Qt::Horizontal, headers[j]);
    }
}

//Определение характеристик таблицы
void chooseVehAccDlg::createUI()
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
void chooseVehAccDlg::on_tableView_doubleClicked(const QModelIndex &index)
{
    QModelIndex chosen = index;
    int id = chosen.sibling(chosen.row(), 0).data().toInt();
    emit sendData(id);
    this->close();
}

void chooseVehAccDlg::on_btn_search_clicked()
{
    model->setQuery(QString("SELECT * FROM (SELECT `vehicle_id`, `brand_name`, `vehicle_registration_tag`, `vin_code` FROM (SELECT `vehicle_id`, "
                            "`brand_name`, `t`.`vehicle_registration_tag`, `vin_code`, `removal_date` "
                            "FROM (SELECT `v`.`vehicle_id`, `brand_name`, `vin_code`, `tag_id`, `removal_date` FROM `vehicle` "
                            "as `v` INNER JOIN `vehicle_registration` as `r` ON `v`.`vehicle_id` = `r`.`vehicle_id`) "
                            "as `one` INNER JOIN `vehicle_registration_tag` as `t` ON `t`.`tag_id` = `one`.`tag_id`) as `two` "
                            "WHERE `removal_date` IS NULL) as `res` WHERE `res`.`vehicle_registration_tag` LIKE'%%1%'").arg(ui->lineEditsearch->text()));
}

void chooseVehAccDlg::on_btn_reset_clicked()
{
    model->setQuery("SELECT `vehicle_id`, `brand_name`, `vehicle_registration_tag`, `vin_code` FROM (SELECT `vehicle_id`, "
                    "`brand_name`, `t`.`vehicle_registration_tag`, `vin_code`, `removal_date` "
                    "FROM (SELECT `v`.`vehicle_id`, `brand_name`, `vin_code`, `tag_id`, `removal_date` FROM `vehicle` "
                    "as `v` INNER JOIN `vehicle_registration` as `r` ON `v`.`vehicle_id` = `r`.`vehicle_id`) "
                    "as `one` INNER JOIN `vehicle_registration_tag` as `t` ON `t`.`tag_id` = `one`.`tag_id`) as `res` "
                    "WHERE `removal_date` IS NULL");
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}
