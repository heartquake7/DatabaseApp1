#include "addaccdialog.h"
#include "ui_addaccdialog.h"

addAccDialog::addAccDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addAccDialog)
{
    ui->setupUi(this);
    ui->btn_choose->setVisible(false);
    chosenVehId = -1;

    //Органичение на максимальное кол-во символов для поля ввода
    ui->lineEdit_1->setMaxLength(8);
    ui->lineEdit_2->setMaxLength(40);
    ui->lineEdit_3->setMaxLength(17);
    ui->lineEdit_4->setMaxLength(20);
    ui->lineEdit_cause->setMaxLength(30);
    ui->lineEdit_cond->setMaxLength(20);
    ui->lineEdit_desc->setMaxLength(150);
    ui->lineEdit_numb->setMaxLength(3);
    ui->lineEdit_street->setMaxLength(30);
    ui->lineEdit_time->setMaxLength(20);

    //Установка допустимых символов для полей ввода с применением регулярных выражений
    ui->lineEdit_1->setValidator(new QRegExpValidator(QRegExp("[0-9А-Я]{8}"), this));
    ui->lineEdit_3->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+"), this));
    ui->lineEdit_numb->setValidator(new QRegExpValidator(QRegExp("[0-9]{3}"), this));
    ui->lineEdit_5->setValidator(new QRegExpValidator(QRegExp("[0-9]{7}"), this));

    ui->dateEdit->setDisplayFormat("yyyy-MM-dd");
    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

addAccDialog::~addAccDialog()
{
    delete ui;
}

void addAccDialog::on_lineEdit_3_textEdited(const QString &arg1)
{
    ui->lineEdit_3->setText(ui->lineEdit_3->text().toUpper());
}

bool addAccDialog::checkfillacc()
{
    bool ok = true;
    m_list.append(ui->lineEdit_cause);
    m_list.append(ui->lineEdit_cond);
    m_list.append(ui->lineEdit_desc);
    m_list.append(ui->lineEdit_numb);
    m_list.append(ui->lineEdit_street);
    m_list.append(ui->lineEdit_time);
    for (int i = 0; i < m_list.size(); i++)
    {
        if (m_list.at(i)->text().isEmpty() && m_list.at(i)->isVisible())
            ok = false;
    }
    m_list.clear();
    return ok;
}

bool addAccDialog::checkfillpart()
{
    bool ok = true;
    m_list.append(ui->lineEdit_1);
    m_list.append(ui->lineEdit_2);
    m_list.append(ui->lineEdit_3);
    m_list.append(ui->lineEdit_4);
    m_list.append(ui->lineEdit_5);
    for (int i = 0; i < m_list.size(); i++)
    {
        if (m_list.at(i)->text().isEmpty() && m_list.at(i)->isVisible())
            ok = false;
    }
    m_list.clear();
    return ok;
}

void addAccDialog::on_btn_submit_clicked()
{
    if (!checkfillacc())
    {
        QMessageBox::warning(this, "Предупреждение", "Необходимо заполнить все поля");
        return;
    }
    if (ui->listWidget->count() == 0)
    {
        QMessageBox::warning(this, "Предупреждение", "Отсутствуют участники ДТП");
        return;
    }
    QSqlQuery query;
    query.exec("START TRANSACTION");
    if (!query.exec("INSERT INTO `place_of_accident`(`place_category_id`) VALUES ('1')"))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
        query.exec("ROLLBACK");
        return;
    }
    query.exec("SELECT LAST_INSERT_ID()");
    query.next();
    int placeID = query.value(0).toInt();
    query.prepare("INSERT INTO `settlement`(`place_id`, `street`, `house_number`) VALUES (:id, :str, :numb)");
    query.bindValue(":id", placeID);
    query.bindValue(":str", ui->lineEdit_street->text());
    query.bindValue(":numb", ui->lineEdit_numb->text());
    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
        query.exec("ROLLBACK");
        return;
    }
    query.prepare("INSERT INTO `road_accident`(`accident_date`, `road_conditions`, `time_of_day`, `accident_description`, "
                  "`cause_of_accident`, `type_of_accident_id`, `place_id`) VALUES (:date, :cond, :time, :desc, "
                  ":cause, :type, :placeid)");
    query.bindValue(":date", ui->dateEdit->text());
    query.bindValue(":cond", ui->lineEdit_cond->text());
    query.bindValue(":time", ui->lineEdit_time->text());
    query.bindValue(":desc", ui->lineEdit_desc->text());
    query.bindValue(":cause", ui->lineEdit_cause->text());
    query.bindValue(":placeid", placeID);
    query.bindValue(":type", ui->comboBox_type->currentIndex() + 1);
    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
        query.exec("ROLLBACK");
        return;
    }
    query.exec("SELECT LAST_INSERT_ID()");
    query.next();
    int accID = query.value(0).toInt();
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QMap<QListWidgetItem*, QStringList>::const_iterator it = map.lowerBound(ui->listWidget->item(i));
        QStringList data = *it;
        int cat = (data.first()).toInt() + 1;
        query.prepare("INSERT INTO `participant_of_accident`(`participant_category_id`) VALUES (:catid)");
        query.bindValue(":catid", cat);
        if (!query.exec())
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
            query.exec("ROLLBACK");
            return;
        }
        query.exec("SELECT LAST_INSERT_ID()");
        query.next();
        int partID = query.value(0).toInt();
        int extID = 0;
        switch (cat)
        {
        case 1:
        {
            query.prepare("INSERT INTO `unregistered_vehicle`(`participant_id`, `vehicle_registration_tag`, "
                          "`vehicle_brand`, `VIN_code`, `vehicle_color`) VALUES (:id, :tag, :brand, :vin, :colour)");
            query.bindValue(":id", partID);
            query.bindValue(":tag", data.at(1));
            query.bindValue(":brand", data.at(2));
            query.bindValue(":vin", data.at(3));
            query.bindValue(":colour", data.at(4));
            extID = (data.at(5)).toInt();
            break;
        }
        case 2:
        {
            query.prepare("INSERT INTO `registered_vehicle`(`participant_id`, `vehicle_id`) "
                           "VALUES (:id, :vehid)");
            query.bindValue(":id", partID);
            query.bindValue(":vehid", data.at(1));
            extID = (data.at(2).toInt());
            break;
        }
        case 3:
        {
            query.prepare("INSERT INTO `pedestrian`(`participant_id`, `full_name`, "
                          "`residence_adress`, `phone_number`) VALUES (:id, :name, :addr, :tel)");
            query.bindValue(":id", partID);
            query.bindValue(":name", data.at(1));
            query.bindValue(":addr", data.at(2));
            query.bindValue(":tel", data.at(3));
            extID = (data.at(4).toInt());
            break;
        }
        default:
            break;
        }

        if (!query.exec())
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
           query.exec("ROLLBACK");
            return;
        }
        query.prepare("INSERT INTO `participation_in_accident`(`loss`, `extent_id`, `accident_id`, `participant_id`) "
                      "VALUES (:loss, :ext, :accid, :partid)");
        query.bindValue(":loss", (data.last()).toInt());
        query.bindValue(":ext", extID);
        query.bindValue(":accid", accID);
        query.bindValue(":partid", partID);
        if (!query.exec())
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
            query.exec("ROLLBACK");
            return;
        }
    }
    query.exec("COMMIT");
    QMessageBox::information(this, "Запись добавлена", "Запись успешно добавлена");
    emit(closethis());
    this->close();
}

void addAccDialog::on_btn_choose_clicked()
{
    chooseVeh = new chooseVehAccDlg(this);
    connect(chooseVeh, SIGNAL(sendData(int)), this, SLOT(receiveDataVeh(int)));
    chooseVeh->show();
}

void addAccDialog::on_btn_add_clicked()
{
    if (!checkfillpart())
    {
        QMessageBox::warning(this, "Предупреждение", "Необходимо заполнить все поля");
        return;
    }
    switch(ui->comboBox->currentIndex())
    {
    case 0:
    {
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(ui->lineEdit_2->text() + ", гос. номер " + ui->lineEdit_1->text() + ", сумма ущерба: " + ui->lineEdit_5->text());
        ui->listWidget->addItem(item);
        QStringList list;
        list << QString::number(ui->comboBox->currentIndex()) << ui->lineEdit_1->text()
             << ui->lineEdit_2->text() << ui->lineEdit_3->text() << ui->lineEdit_4->text()
             << QString::number(ui->comboBox_ext->currentIndex() + 1)<< ui->lineEdit_5->text();
        map.insert(item, list);
        break;
    }
    case 1:
    {
        QSqlQuery query;
        query.prepare("SELECT * FROM (SELECT `vehicle_id`, `brand_name`, `t`.`vehicle_registration_tag`, "
                      "`vin_code` FROM (SELECT * FROM (SELECT `v`.`vehicle_id`, `brand_name`, `vin_code`, "
                      "`tag_id`, `removal_date` FROM `vehicle` as `v` INNER JOIN `vehicle_registration` as "
                      "`r` ON `v`.`vehicle_id` = `r`.`vehicle_id`) as `res` WHERE `res`.`removal_date` "
                      "IS NULL) as `one` INNER JOIN `vehicle_registration_tag` as `t` ON `t`.`tag_id` = "
                      "`one`.`tag_id`) as `two` WHERE `vehicle_id` = :id");
        query.bindValue(":id", chosenVehId);
        if (!query.exec())
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось загрузить данные. Проверьте соединение с базой данных");
            return;
        }
        query.next();
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(query.value(1).toString() + ", гос. номер " + query.value(2).toString() + ", сумма ущерба: " + ui->lineEdit_5->text());
        ui->listWidget->addItem(item);
        QStringList list;
        list << QString::number(ui->comboBox->currentIndex()) << query.value(0).toString()
             << QString::number(ui->comboBox_ext->currentIndex() + 1) << ui->lineEdit_5->text();
        map.insert(item, list);
        break;
    }
    case 2:
    {
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(ui->lineEdit_1->text() + ", сумма ущерба: " + ui->lineEdit_5->text());
        ui->listWidget->addItem(item);
        QStringList list;
        list << QString::number(ui->comboBox->currentIndex()) << ui->lineEdit_1->text()
             << ui->lineEdit_2->text() << ui->lineEdit_3->text()
             << QString::number(ui->comboBox_ext->currentIndex() + 1) << ui->lineEdit_5->text();
        map.insert(item, list);
        break;
    }
     default:
        break;
    }
    ui->lineEdit_1->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();
}

void addAccDialog::on_btn_delete_clicked()
{
   QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
   foreach (QListWidgetItem* item, items)
   {
       ui->listWidget->removeItemWidget(item);
       map.erase(map.find(item));
       delete item;
   }
}

void addAccDialog::on_comboBox_currentIndexChanged(int index)
{
    //Обработка переключений категории участника ДТП
    ui->lineEdit_1->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();
    switch (index)
    {
    case 0:
    {
        chosenVehId = -1;
        ui->label_1->setVisible(true);
        ui->lineEdit_1->setVisible(true);
        ui->label_2->setVisible(true);
        ui->lineEdit_2->setVisible(true);
        ui->label_3->setVisible(true);
        ui->lineEdit_3->setVisible(true);
        ui->label_4->setVisible(true);
        ui->lineEdit_4->setVisible(true);
        ui->btn_choose->setVisible(false);
        ui->btn_add->setVisible(true);
        ui->label_1->setText(trUtf8("Гос. номер"));
        ui->label_2->setText(trUtf8("Марка ТС"));
        ui->label_3->setText(trUtf8("VIN-код"));
        ui->label_4->setText(trUtf8("Цвет ТС"));
        ui->lineEdit_1->setValidator(new QRegExpValidator(QRegExp("[0-9А-Я]{8}"), this));
        ui->lineEdit_2->setValidator(new QRegExpValidator(QRegExp("[А-Яа-яA-Za-z0-9- ]+"), this));
        ui->lineEdit_3->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+"), this));
        break;
    }
    case 1:
    {
        chosenVehId = -1;
        ui->label_1->setVisible(false);
        ui->lineEdit_1->setVisible(false);
        ui->label_2->setVisible(false);
        ui->lineEdit_2->setVisible(false);
        ui->label_3->setVisible(false);
        ui->lineEdit_3->setVisible(false);
        ui->label_4->setVisible(false);
        ui->lineEdit_4->setVisible(false);
        ui->btn_choose->setVisible(true);
        ui->btn_add->setVisible(false);

        break;
    }
    case 2:
    {
        chosenVehId = -1;
        ui->label_1->setVisible(true);
        ui->lineEdit_1->setVisible(true);
        ui->label_2->setVisible(true);
        ui->lineEdit_2->setVisible(true);
        ui->label_3->setVisible(true);
        ui->lineEdit_3->setVisible(true);
        ui->lineEdit_4->setVisible(false);
        ui->label_4->setVisible(false);
        ui->btn_choose->setVisible(false);
        ui->btn_add->setVisible(true);
        ui->label_1->setText(trUtf8("ФИО"));
        ui->label_2->setText(trUtf8("Адрес проживания"));
        ui->label_3->setText(trUtf8("Номер телефона"));
        ui->lineEdit_1->setValidator(new QRegExpValidator(QRegExp("[А-Яа-я ]+"), this));
        ui->lineEdit_2->setValidator(new QRegExpValidator(QRegExp("[А-Яа-я0-9-,. ]+"), this));
        ui->lineEdit_3->setValidator(new QRegExpValidator(QRegExp("[0-9]+"), this));
        break;
    }
    default:
        break;
    }
}

//Слот обработки сигнала выбранного ТС из базы
void addAccDialog::receiveDataVeh(int ID)
{
    chosenVehId = ID;
    ui->btn_add->setVisible(true);
}
