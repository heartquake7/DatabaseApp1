#include "choosevehdlg.h"
#include "ui_choosevehdlg.h"

chooseVehDlg::chooseVehDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chooseVehDlg)
{
    ui->setupUi(this);
    this->setupModel(QStringList() << trUtf8("ID") << trUtf8("Транспортное средство") << trUtf8("Дата выпуска")
                     << trUtf8("Мощность двигателя")
                     << trUtf8("VIN-код") << trUtf8("Цвет ТС"));
    this->createUI();
}

chooseVehDlg::~chooseVehDlg()
{
    delete ui;
}

void chooseVehDlg::setupModel(const QStringList &headers)
{
    model = new QSqlQueryModel(this);

    //Запрос на выборку снятых с учета ТС
    model->setQuery("SELECT `v`.`Vehicle_id`, `brand_name`, `manufacture_year`, `engine_capacity_hp`, `vin_code`, "
                    "`vehicle_colour` FROM `vehicle` as `v` JOIN (SELECT `one`.`vehicle_id` FROM "
                    "(SELECT `vehicle_id` FROM `vehicle_registration` GROUP BY `vehicle_id` HAVING "
                    "COUNT(`vehicle_id`) = 1) as `one` JOIN `vehicle_registration` ON `vehicle_registration`.`vehicle_id` "
                    "= `one`.`vehicle_id` WHERE `vehicle_registration`.`removal_date` is not null "
                    "UNION SELECT `vehicle_id` FROM `vehicle_registration` WHERE `vehicle_id` NOT IN"
                    "(SELECT `one`.`vehicle_id` FROM (SELECT `vehicle_id` FROM `vehicle_registration` "
                    "GROUP BY `vehicle_id` HAVING COUNT(`vehicle_id`) > 1) as `one` JOIN (SELECT `vehicle_id` "
                    "FROM `vehicle_registration` WHERE `removal_date` is null) as `two` ON `one`.`vehicle_id` = "
                    "`two`.`vehicle_id`) GROUP BY `vehicle_id` HAVING COUNT(`vehicle_id`) > 1) as `vx` ON "
                    "`v`.`vehicle_id` = `vx`.`vehicle_id`");

    for (int i = 0, j = 0; i < model->columnCount(); i++, j++)
    {
        model->setHeaderData(i, Qt::Horizontal, headers[j]);
    }
}

//Определение характеристик таблицы
void chooseVehDlg::createUI()
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
void chooseVehDlg::on_tableView_doubleClicked(const QModelIndex &index)
{
    QModelIndex chosen = index;
    int id = chosen.sibling(chosen.row(), 0).data().toInt();
    emit sendData(id);
    this->close();
}
