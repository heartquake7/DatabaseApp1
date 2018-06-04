#include "regstartwnd.h"
#include "ui_regstartwnd.h"

RegStartWnd::RegStartWnd(QWidget *parent, database* p_database, access acc) :
    QWidget(parent),
    ui(new Ui::RegStartWnd)
{
    ui->setupUi(this);
    //Скрыть ненужные в данный момент элементы интерфейса
    ui->label_vehicle->setVisible(false);
    ui->lineEdit_vehicle->setVisible(false);
    ui->label_tag->setVisible(false);
    ui->lineEdit_tag->setVisible(false);
    ui->label_vin->setVisible(false);
    ui->lineEdit_vin->setVisible(false);
    ui->label_owner->setVisible(false);
    ui->lineEdit_owner->setVisible(false);
    ui->btn_search->setVisible(false);
    ui->btn_reset->setVisible(false);
    ui->btn_applyfilter->setVisible(false);
    ui->btn_resetfilter->setVisible(false);
    ui->dateEditfrom->setVisible(false);
    ui->dateEdit_to->setVisible(false);
    ui->label_datefrom->setVisible(false);
    ui->label_dateto->setVisible(false);

    //Установка допустимых значений для полей ввода с помощью регулярных выражений
    ui->lineEdit_vehicle->setValidator(new QRegExpValidator(QRegExp("[А-Яа-яA-Za-z0-9- ]+"), this));
    ui->lineEdit_tag->setValidator(new QRegExpValidator(QRegExp("[0-9А-Яа-я]{6}"), this));
    ui->lineEdit_vin->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+"), this));
    ui->lineEdit_owner->setValidator(new QRegExpValidator(QRegExp("[А-Яа-я0-9- ]+"), this));
    db = p_database;
    Parent = parent;
    Acc = acc;

    //Проверка прав доступа
    switch (Acc)
    {
        case admin:
        ui->label_acc->setText(ui->label_acc->text() + "Администратор");
        break;

        case guest:
        ui->btn_add->setHidden(true);
        ui->label_acc->setText(ui->label_acc->text() + "Гость");
        break;

        default:
        break;
    }
    this->setupModel(QStringList() << trUtf8("ID") << trUtf8("Транспортное средство") << trUtf8("VIN-код")
                     << trUtf8("Владелец")
                     << trUtf8("Дата регистрации")
                     << trUtf8("Дата снятия с учета") << trUtf8("Гос. Номер"));
    this->createUI();
}

RegStartWnd::~RegStartWnd()
{
    delete ui;
}

void RegStartWnd::on_btn_back_clicked()
{
    this->close();
}

void RegStartWnd::setupModel(const QStringList &headers)
{
    model = new QSqlQueryModel(this);

    //Запрос на выборку регистраций из базы
    query.exec("SELECT `Registration_ID`, `Brand_name`, `Vin_code`, CASE `Owner_category_ID`"
               " WHEN '1' THEN `ФИО Владельца` WHEN '2' THEN `Компания-владелец` "
               "ELSE NULL END as `owner`, `Registration_date`, `Removal_date`, `Vehicle_registration_tag` FROM `vehicle_registration` "
               "AS  `r` ,  `vehicle_registration_tag` AS  `t` ,  "
               "`vehicle` AS  `v`, `vehicle_owner` as `o` , (SELECT  `v`.`owner_id` AS  `Номер_владельца` ,  "
               "`i`.`full_name` AS  `ФИО Владельца` , NULL AS  `Компания-владелец` FROM  `vehicle_owner` AS  `v` "
               "INNER JOIN  `individual` AS  `i` ON  `v`.`owner_id` =  `i`.`owner_id` "
               "WHERE  `v`.`owner_category_id` IN (SELECT  `owner_category_id` FROM  `individual` WHERE 1) "
               "UNION SELECT  `v`.`owner_id` AS  `Номер_владельца` , NULL AS  `ФИО Владельца` ,  `l`.`company_name` AS  `Компания-владелец`"
               "FROM  `vehicle_owner` AS  `v` INNER JOIN  `legal_entity` AS  `l` ON  `v`.`owner_id` =  `l`.`owner_id` "
               "WHERE  `v`.`owner_category_id` IN (SELECT  `owner_category_id` FROM  `legal_entity` WHERE 1)) AS  `one` "
               "WHERE  `r`.`vehicle_id` =  `v`.`vehicle_id` AND  `r`.`tag_id` =  `t`.`tag_id` "
               "AND  `one`.`номер_владельца` =  `r`.`owner_id` AND `o`.`Owner_ID` = `r`.`Owner_ID`"
                               "ORDER BY `Registration_date` DESC");
    model->setQuery(query);

    for (int i = 0, j = 0; i < model->columnCount(); i++, j++)
    {
        model->setHeaderData(i, Qt::Horizontal, headers[j]);
    }
}

void RegStartWnd::createUI()
{
     ui->tableView->setModel(model);
     ui->tableView->setColumnHidden(0, true);
     ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
     ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
     ui->tableView->resizeColumnsToContents();
     ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
     ui->tableView->horizontalHeader()->setStretchLastSection(true);
     ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
     ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
     if (Acc == admin)
     {
        //Разрешить использование контекстного меню, если права доступа - администратор
        connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotEditRecord()));
        connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuReq(QPoint)));
     }
}

//Обработка вызова контекстного меню
void RegStartWnd::slotCustomMenuReq(QPoint pos)
{
    switch (Acc)
    {
    case guest:
        break;

    case admin:
    {
        //Создание элементов контексного меню
        QMenu* menu = new QMenu(this);
        QAction* editRec = new QAction(trUtf8("Снять с учета"), this);
        QAction* deleteRec = new QAction(trUtf8("Удалить"), this);
        connect(editRec, SIGNAL(triggered()), this, SLOT(slotEditRecord()));
        connect(deleteRec, SIGNAL(triggered()), this, SLOT(slotDeleteRecord()));
        menu->addAction(editRec);
        menu->addAction(deleteRec);
        menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
        break;
    }
    default:
        break;
    }
}

//Снятие ТС с учёта
void RegStartWnd::slotEditRecord()
{
    QObject* obj = sender();
    if (obj == ui->tableView)
        return;
    int row = ui->tableView->selectionModel()->currentIndex().row();
    if (row >= 0)
    {
       QMessageBox mb(trUtf8("Снятие с учета"), trUtf8("Подтвердить снятие транспортного средства с учета?"),
                       QMessageBox::Question, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
       mb.setButtonText(QMessageBox::Yes, trUtf8("Да"));
       mb.setButtonText(QMessageBox::No, trUtf8("Отмена"));
       if (mb.exec() == QMessageBox::No)
           return;
       else
       {
           QModelIndex current = ui->tableView->currentIndex();
           int id = current.sibling(current.row(), 0).data().toInt();
           QSqlQuery query;
           query.prepare("SELECT * FROM `vehicle_registration` WHERE `Registration_ID` = :id "
                         "AND `removal_date` IS NULL");
           query.bindValue(":id", id);
           query.exec();
           if (!query.next())
           {
               QMessageBox::critical(this, "Ошибка", "Транспортное средство уже снято с учета");
               return;
           }
           query.prepare("UPDATE `vehicle_registration` SET `removal_date` = CURDATE() WHERE `Registration_ID` = :id "
                         "AND `removal_date` IS NULL");
           query.bindValue(":id", id);
           query.exec();
           if (!query.exec())
           {
               QMessageBox::critical(this, trUtf8("Внимание"), trUtf8("Не удалось обновить запись"));
               return;
           }
           else
           {
               QMessageBox::information(this, trUtf8("Успешно"), trUtf8("Транспортное средство снято с учета"));
               model->setQuery(model->query().lastQuery());
               ui->tableView->setCurrentIndex(model->index(-1, -1));
           }
        }
      }
}

//Удаление записи о регистрации
void RegStartWnd::slotDeleteRecord()
{
    int row = ui->tableView->selectionModel()->currentIndex().row();
    if (row >= 0)
    {
       QMessageBox mb(trUtf8("Удалить запись"), trUtf8("Подтвердить удаление записи?"),
                       QMessageBox::Question, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
       mb.setButtonText(QMessageBox::Yes, trUtf8("Да"));
       mb.setButtonText(QMessageBox::No, trUtf8("Отмена"));
       if (mb.exec() == QMessageBox::No)
           return;
       else
       {
           QModelIndex current = ui->tableView->currentIndex();
           int id = current.sibling(current.row(), 0).data().toInt();
           int type;
           int delID;
           QSqlQuery query;
           query.prepare("SELECT `vehicle_id` FROM `vehicle_registration` WHERE `Registration_ID` = :id");
           query.bindValue(":id", id);
           query.exec();
           query.next();
           delID = query.value(0).toInt();
           query.prepare("DELETE FROM `vehicle_registration` WHERE `Registration_ID` = :id");
           query.bindValue(":id", id);
           if (!query.exec())
           {
               QMessageBox::critical(this, trUtf8("Внимание"), trUtf8("Не удалось удалить запись"));
               return;
           }
           query.prepare("SELECT `type_id` FROM `vehicle` WHERE `vehicle_ID` = :id");
           query.bindValue(":id", delID);
           query.exec();
           query.next();
           type = query.value(0).toInt();
           query.prepare("DELETE FROM `vehicle` WHERE `vehicle_ID` = :id");
           query.bindValue(":id", delID);
           query.exec();
           switch(type)
           {
           case 1:
           {
              query.prepare("DELETE FROM `car_transport` WHERE `vehicle_ID` = :id");
              query.bindValue(":id", delID);
              query.exec();
           }
           case 2:
           {
              query.prepare("DELETE FROM `freight_transport` WHERE `vehicle_ID` = :id");
              query.bindValue(":id", delID);
              query.exec();
           }
           case 3:
           {
              query.prepare("DELETE FROM `passenger_transport` WHERE `vehicle_ID` = :id");
              query.bindValue(":id", delID);
              query.exec();
           }
           default:
               break;
           }
           QMessageBox::information(this, trUtf8("Успешно"), trUtf8("Запись успешно удалена"));
           model->setQuery(model->query().lastQuery());
           ui->tableView->setCurrentIndex(model->index(-1, -1));
       }
    }
}

//Обработка нажатия кнопки "Новая регистрация"
void RegStartWnd::on_btn_add_clicked()
{
    addRegDialog* adddlg = new addRegDialog(this);
    connect(adddlg, SIGNAL(closethis()), this, SLOT(oncloseDlg()));
    adddlg->show();
}

void RegStartWnd::oncloseDlg()
{
    model->setQuery(model->query().lastQuery());
}

//Показать элементы интерфейса для осуществления поиска
void RegStartWnd::on_btn_showsearch_clicked()
{
    ui->label_vehicle->setVisible(true);
    ui->lineEdit_vehicle->setVisible(true);
    ui->label_tag->setVisible(true);
    ui->lineEdit_tag->setVisible(true);
    ui->label_vin->setVisible(true);
    ui->lineEdit_vin->setVisible(true);
    ui->label_owner->setVisible(true);
    ui->lineEdit_owner->setVisible(true);
    ui->btn_search->setVisible(true);
    ui->btn_reset->setVisible(true);
}

//Поиск по параметрам
void RegStartWnd::on_btn_search_clicked()
{
    model->setQuery(QString("SELECT * FROM (SELECT `Registration_ID`, `Brand_name`, `Vin_code`, "
                            "CASE `Owner_category_ID` WHEN '1' THEN `ФИО Владельца` WHEN '2' THEN "
                            "`Компания-владелец` ELSE NULL END as `owner`, `Registration_date`, `Removal_date`, "
                            "`Vehicle_registration_tag` FROM `vehicle_registration` AS  `r` ,  `vehicle_registration_tag` "
                            "AS  `t` ,  `vehicle` AS  `v`, `vehicle_owner` as `o` , (SELECT  `v`.`owner_id` AS  "
                            "`Номер_владельца` ,  `i`.`full_name` AS  `ФИО Владельца` , NULL AS  `Компания-владелец` "
                            "FROM  `vehicle_owner` AS  `v` INNER JOIN  `individual` AS  `i` ON  `v`.`owner_id` =  "
                            "`i`.`owner_id` WHERE  `v`.`owner_category_id` IN (SELECT  `owner_category_id` FROM  "
                            "`individual` WHERE 1)  UNION SELECT  `v`.`owner_id` AS  `Номер_владельца` , NULL AS  "
                            "`ФИО Владельца` ,  `l`.`company_name` AS  `Компания-владелец` FROM  `vehicle_owner` "
                            "AS  `v` INNER JOIN  `legal_entity` AS  `l` ON  `v`.`owner_id` =  `l`.`owner_id` WHERE  "
                            "`v`.`owner_category_id` IN (SELECT  `owner_category_id` FROM  `legal_entity` WHERE 1)) "
                            "AS  `one` WHERE  `r`.`vehicle_id` =  `v`.`vehicle_id` AND  `r`.`tag_id` =  `t`.`tag_id` "
                            "AND  `one`.`номер_владельца` =  `r`.`owner_id` AND `o`.`Owner_ID` = `r`.`Owner_ID`  "
                            "ORDER BY `Registration_date` DESC) as `two` WHERE `two`.`owner` LIKE '%%1%' "
                            "AND `vehicle_registration_tag` LIKE '%%2%' AND `brand_name` LIKE '%%3%' "
                            "AND `vin_code` LIKE '%%4%'").arg(ui->lineEdit_owner->text(), ui->lineEdit_tag->text(),
                                                                    ui->lineEdit_vehicle->text(), ui->lineEdit_vin->text()));

}

void RegStartWnd::on_btn_reset_clicked()
{
    ui->lineEdit_vehicle->clear();
    ui->lineEdit_tag->clear();
    ui->lineEdit_vin->clear();
    ui->lineEdit_owner->clear();
    model->setQuery(query);
}

void RegStartWnd::on_tableView_doubleClicked(const QModelIndex &index)
{
    return;
}

//Показать элементы интерфейса, отвечающие за фильтрацию по дате
void RegStartWnd::on_btn_showfilter_clicked()
{
    ui->btn_applyfilter->setVisible(true);
    ui->btn_resetfilter->setVisible(true);
    ui->dateEditfrom->setVisible(true);
    ui->dateEditfrom->setDisplayFormat("yyyy-MM-dd");
    ui->dateEdit_to->setVisible(true);
    ui->dateEdit_to->setDisplayFormat("yyyy-MM-dd");
    ui->label_datefrom->setVisible(true);
    ui->label_dateto->setVisible(true);
}

void RegStartWnd::on_btn_resetfilter_clicked()
{
    model->setQuery(query);
}

//Применить фильтр
void RegStartWnd::on_btn_applyfilter_clicked()
{
    model->setQuery(QString("SELECT * FROM (SELECT * FROM (SELECT `Registration_ID`, `Brand_name`, `Vin_code`, "
                            "CASE `Owner_category_ID` WHEN '1' THEN `ФИО Владельца` WHEN '2' THEN "
                            "`Компания-владелец` ELSE NULL END as `owner`, `Registration_date`, `Removal_date`, "
                            "`Vehicle_registration_tag` FROM `vehicle_registration` AS  `r` ,  `vehicle_registration_tag` "
                            "AS  `t` ,  `vehicle` AS  `v`, `vehicle_owner` as `o` , (SELECT  `v`.`owner_id` AS  "
                            "`Номер_владельца` ,  `i`.`full_name` AS  `ФИО Владельца` , NULL AS  `Компания-владелец` "
                            "FROM  `vehicle_owner` AS  `v` INNER JOIN  `individual` AS  `i` ON  `v`.`owner_id` =  "
                            "`i`.`owner_id` WHERE  `v`.`owner_category_id` IN (SELECT  `owner_category_id` FROM  "
                            "`individual` WHERE 1)  UNION SELECT  `v`.`owner_id` AS  `Номер_владельца` , NULL AS  "
                            "`ФИО Владельца` ,  `l`.`company_name` AS  `Компания-владелец` FROM  `vehicle_owner` "
                            "AS  `v` INNER JOIN  `legal_entity` AS  `l` ON  `v`.`owner_id` =  `l`.`owner_id` WHERE  "
                            "`v`.`owner_category_id` IN (SELECT  `owner_category_id` FROM  `legal_entity` WHERE 1)) "
                            "AS  `one` WHERE  `r`.`vehicle_id` =  `v`.`vehicle_id` AND  `r`.`tag_id` =  `t`.`tag_id` "
                            "AND  `one`.`номер_владельца` =  `r`.`owner_id` AND `o`.`Owner_ID` = `r`.`Owner_ID`  "
                            "ORDER BY `Registration_date` DESC) as `two` WHERE `two`.`owner` LIKE '%%1%' "
                            "AND `vehicle_registration_tag` LIKE '%%2%' AND `brand_name` LIKE '%%3%' "
                            "AND `vin_code` LIKE '%%4%') as `four` WHERE `four`.`registration_date` "
                            "BETWEEN '%5' AND '%6'").arg(ui->lineEdit_owner->text(), ui->lineEdit_tag->text(),
                                                                    ui->lineEdit_vehicle->text(), ui->lineEdit_vin->text(),
                                                         ui->dateEditfrom->text(), ui->dateEdit_to->text()));
}
