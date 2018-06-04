#include "chooseowndlg.h"
#include "ui_chooseowndlg.h"

chooseOwnDlg::chooseOwnDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chooseOwnDlg)
{
    ui->setupUi(this);
    this->setupModel(QStringList() << trUtf8("ID") << trUtf8("Владелец") << trUtf8("Адрес")
                     << trUtf8("Телефон"));
    this->createUI();
}

chooseOwnDlg::~chooseOwnDlg()
{
    delete ui;
}

void chooseOwnDlg::setupModel(const QStringList &headers)
{
    model = new QSqlQueryModel(this);

    //Запрос на выборку данных о владельцах
    model->setQuery("SELECT `o`.`owner_id`, CASE `o`.`owner_category_id` WHEN 1 THEN `full_name` "
                    "ELSE `company_name` END as `owner`, CASE `o`.`owner_category_id` WHEN 1 THEN "
                    "`registration_adress` ELSE `legal_adress` END as `address`, `phone_number` FROM  "
                    "`vehicle_owner` as `o` LEFT JOIN `individual` as `i` ON `o`.`owner_id` = `i`.`owner_id` "
                    "LEFT JOIN `legal_entity` as `l` ON `o`.`owner_id`= `l`.`owner_id`");

    for (int i = 0, j = 0; i < model->columnCount(); i++, j++)
    {
        model->setHeaderData(i, Qt::Horizontal, headers[j]);
    }
}

void chooseOwnDlg::createUI()
{
      // Определение характеристик таблицы
      ui->tableView->setModel(model);
      ui->tableView->setColumnHidden(0, true);
      ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
      ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
      ui->tableView->resizeColumnsToContents();
      ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
      ui->tableView->horizontalHeader()->setStretchLastSection(true);
      ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

// Слот обработки двойного клика на строке таблицы
void chooseOwnDlg::on_tableView_doubleClicked(const QModelIndex &index)
{
    QModelIndex chosen = index;
    int id = chosen.sibling(chosen.row(), 0).data().toInt();
    emit sendData(id);
    this->close();
}

//Слот обработки нажатия кнопки "Поиск"
void chooseOwnDlg::on_btn_search_clicked()
{
    model->setQuery(QString("SELECT * FROM (SELECT `o`.`owner_id`, CASE `o`.`owner_category_id` WHEN 1 "
                    "THEN `full_name` ELSE `company_name` END as `owner`, CASE `o`.`owner_category_id` "
                    "WHEN 1 THEN `registration_adress` ELSE `legal_adress` END as `address`, `phone_number` "
                    "FROM `vehicle_owner` as `o` LEFT JOIN `individual` as `i` ON `o`.`owner_id` = "
                    "`i`.`owner_id` LEFT JOIN `legal_entity` as `l` ON `o`.`owner_id`= `l`.`owner_id`) "
                    "as `res` WHERE `res`.`owner` LIKE '%%1%'").arg(ui->lineEditsearch->text()));
}

//Обработка кнопки "Сброс"
void chooseOwnDlg::on_btn_reset_clicked()
{
    model->setQuery("SELECT `o`.`owner_id`, CASE `o`.`owner_category_id` WHEN 1 THEN `full_name` "
                    "ELSE `company_name` END as `owner`, CASE `o`.`owner_category_id` WHEN 1 THEN "
                    "`registration_adress` ELSE `legal_adress` END as `address`, `phone_number` FROM  "
                    "`vehicle_owner` as `o` LEFT JOIN `individual` as `i` ON `o`.`owner_id` = `i`.`owner_id` "
                    "LEFT JOIN `legal_entity` as `l` ON `o`.`owner_id`= `l`.`owner_id`");
    ui->lineEditsearch->clear();
}
