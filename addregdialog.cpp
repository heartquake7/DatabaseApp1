#include "addregdialog.h"
#include "ui_addregdialog.h"

addRegDialog::addRegDialog(QWidget *parent, database* p_database) :
    QDialog(parent),
    ui(new Ui::addRegDialog)
{
    ui->setupUi(this);
    db = p_database;
    chosenOwnId = -1;
    chosenTagId = -1;
    chosenVehId = -1;

    //Органичение на максимальное кол-во символов для поля ввода
    ui->lineEdit_brand->setMaxLength(40);
    ui->lineEdit_color->setMaxLength(20);
    ui->lineEdit_vin->setMaxLength(30);
    ui->lineEdit_extra->setMaxLength(30);
    ui->lineEdit_fullname->setMaxLength(80);
    ui->lineEdit_addr->setMaxLength(100);
    ui->lineEdit_telephone->setMaxLength(50);
    ui->lineEdit_phaddr->setMaxLength(100);
    ui->lineEdit_chief->setMaxLength(80);

    //Установка допустимых символов для полей ввода с применением регулярных выражений
    ui->lineEdit_year->setValidator(new QRegExpValidator(QRegExp("[0-9]{4}"), this));
    ui->lineEdit_color->setValidator(new QRegExpValidator(QRegExp("[А-Яа-я- ]+"), this));
    ui->lineEdit_vin->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]+"), this));
    ui->lineEdit_hp->setValidator(new QRegExpValidator(QRegExp("[0-9]{4}"), this));
    ui->lineEdit_extra->setValidator(new QRegExpValidator(QRegExp("[А-Яа-я- ]+"), this));
    ui->lineEdit_fullname->setValidator(new QRegExpValidator(QRegExp("[А-Яа-я ]+"), this));
    ui->lineEdit_telephone->setValidator(new QRegExpValidator(QRegExp("[0-9, ]+"), this));
    ui->lineEdit_chief->setValidator(new QRegExpValidator(QRegExp("[А-Яа-я ]+"), this));
    ui->label_phaddr->setVisible(false);
    ui->lineEdit_phaddr->setVisible(false);
    ui->label_chief->setVisible(false);
    ui->lineEdit_chief->setVisible(false);
}

addRegDialog::~addRegDialog()
{
    delete ui;
}

void addRegDialog::on_comboBoxtype_currentIndexChanged(int index)
{
    //Обработка переключений типа транспортного средства
    switch (index)
    {
    case 0:
    {
        ui->label_extra->setText(trUtf8("Тип кузова"));
        ui->lineEdit_extra->setValidator(new QRegExpValidator(QRegExp("[А-Яа-я- ]+"), this));
        break;
    }
    case 1:
    {
        ui->label_extra->setText(trUtf8("Способ разгрузки"));
        ui->lineEdit_extra->setValidator(new QRegExpValidator(QRegExp("[А-Яа-я- ]+"), this));
        break;
    }
    case 2:
    {
        ui->label_extra->setText(trUtf8("Кол-во мест для сидения"));
        ui->lineEdit_extra->clear();
        ui->lineEdit_extra->setValidator(new QRegExpValidator(QRegExp("[0-9]{3}"), this));
        break;
    }
    default:
        break;
    }
}

void addRegDialog::on_comboBoxowner_currentIndexChanged(int index)
{
    //Обработка переключений категорий владельцев
    switch (index)
    {
    case 0:
    {
        ui->label_addr->setText(trUtf8("Адрес прописки"));
        ui->label_phaddr->setVisible(false);
        ui->lineEdit_phaddr->setVisible(false);
        ui->label_chief->setVisible(false);
        ui->lineEdit_chief->setVisible(false);
        break;
    }
    case 1:
    {
        ui->label_addr->setText(trUtf8("Юридический адрес"));
        ui->label_phaddr->setVisible(true);
        ui->lineEdit_phaddr->setVisible(true);
        ui->label_chief->setVisible(true);
        ui->lineEdit_chief->setVisible(true);
        break;
    }
    default:
        break;
    }
}

void addRegDialog::on_lineEdit_vin_textEdited(const QString &arg1)
{
    ui->lineEdit_vin->setText(ui->lineEdit_vin->text().toUpper());
}

void addRegDialog::on_btn_choosevehicle_clicked()
{
    //Вызов диалога выбора транспортного средства из базы
    chooseVeh = new chooseVehDlg(this);
    connect(chooseVeh, SIGNAL(sendData(int)), this, SLOT(receiveDataVeh(int)));
    chooseVeh->show();
}

void addRegDialog::on_btn_chooseowner_clicked()
{
    //Вызов диалога выбора владельца из базы
    chooseOwn = new chooseOwnDlg(this);
    connect(chooseOwn, SIGNAL(sendData(int)), this, SLOT(receiveDataOwn(int)));
    chooseOwn->show();
}

void addRegDialog::on_btn_choosetag_clicked()
{
    //Вызов диалога выбора гос. номера из базы
    chooseTag = new chooseTagDlg(this);
    connect(chooseTag, SIGNAL(sendData(int)), this, SLOT(receiveDataTag(int)));
    chooseTag->show();
}

void addRegDialog::on_btn_cancelvehicle_clicked()
{
    //Сброс выбранного ТС
    chosenVehId = -1;
    ui->lineEdit_brand->clear();
    ui->lineEdit_brand->setReadOnly(false);
    ui->lineEdit_year->clear();
    ui->lineEdit_year->setReadOnly(false);
    ui->lineEdit_vin->clear();
    ui->lineEdit_vin->setReadOnly(false);
    ui->lineEdit_hp->clear();
    ui->lineEdit_hp->setReadOnly(false);
    ui->lineEdit_color->clear();
    ui->lineEdit_color->setReadOnly(false);
    ui->lineEdit_extra->setVisible(true);
    ui->label_extra->setVisible(true);
    ui->comboBoxtype->setEnabled(true);

}

void addRegDialog::on_btn_cancelowner_clicked()
{
    //Сброс выбранного владельца
    chosenOwnId = -1;
    ui->lineEdit_fullname->clear();
    ui->lineEdit_fullname->setReadOnly(false);
    ui->lineEdit_telephone->clear();
    ui->lineEdit_telephone->setReadOnly(false);
    ui->lineEdit_addr->clear();
    ui->lineEdit_addr->setReadOnly(false);
    ui->comboBoxowner->setEnabled(true);
}

void addRegDialog::on_btn_submit_clicked()
{
    //Обработка нажатие кнопки "Добавить запись"
    if (!checkfill())
    {
        QMessageBox::warning(this, "Предупреждение", "Необходимо заполнить все поля");
        return;
    }
    QSqlQuery query;
    query.exec("START TRANSACTION");

    //Если и ТС, и владелец выбраны из базы
    if (chosenVehId != -1 && chosenOwnId != -1)
    {
        query.prepare("INSERT INTO `vehicle_registration`(`vehicle_id`, `owner_id`, `tag_id`, `registration_date`, "
                      "`removal_date`) VALUES (:vehid, :ownid, :tagid, CURDATE(), NULL)");
        query.bindValue(":vehid", chosenVehId);
        query.bindValue(":ownid", chosenOwnId);
        query.bindValue(":tagid", chosenTagId);
        if (!query.exec())
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
            query.exec("ROLLBACK");
            return;
        }
    }

    //Если данные ТС получены из поля ввода, а владелец выбран из базы
    else if (chosenVehId == -1 && chosenOwnId != -1)
    {
        query.prepare("INSERT INTO `vehicle`(`brand_name`, `manufacture_year`, `engine_capacity_hp`, `VIN_code`, "
                      "`vehicle_colour`, `type_id`) VALUES (:name, :year, :hp, :vin, :col, :type)");
        query.bindValue(":name", ui->lineEdit_brand->text());
        query.bindValue(":year", ui->lineEdit_year->text().toInt());
        query.bindValue(":hp", ui->lineEdit_hp->text().toInt());
        query.bindValue(":vin", ui->lineEdit_vin->text());
        query.bindValue(":col", ui->lineEdit_color->text());
        query.bindValue(":type", ui->comboBoxtype->currentIndex() + 1);
        if (!query.exec())
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
            query.exec("ROLLBACK");
            return;
        }
        query.exec("SELECT LAST_INSERT_ID()");
        query.next();
        int id = query.value(0).toInt();
        switch (ui->comboBoxtype->currentIndex())
        {
        case 0:
        {
            query.prepare("INSERT INTO `car_transport`(`vehicle_id`, `body_type`) VALUES (:id, :type)");
            query.bindValue(":id", id);
            query.bindValue(":type", ui->lineEdit_extra->text());
            if (!query.exec())
            {
                QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
                query.exec("ROLLBACK");
                return;
            }
            break;
        }
        case 1:
        {
            query.prepare("INSERT INTO `freight_transport`(`vehicle_id`, `unloading_mode`) VALUES (:id, :mode)");
            query.bindValue(":id", id);
            query.bindValue(":mode", ui->lineEdit_extra->text());
            if (!query.exec())
            {
                QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
                query.exec("ROLLBACK");
                return;
            }
            break;
        }
        case 2:
        {
            query.prepare("INSERT INTO `passenger_transport`(`vehicle_id`, `number_of_seats`) VALUES (:id, :number)");
            query.bindValue(":id", id);
            query.bindValue(":number", ui->lineEdit_extra->text().toInt());
            if (!query.exec())
            {
                QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
                query.exec("ROLLBACK");
                return;
            }
            break;
        }
        default:
            break;
        }
        query.prepare("INSERT INTO `vehicle_registration`(`vehicle_id`, `owner_id`, `tag_id`, `registration_date`, "
                      "`removal_date`) VALUES (:vehid, :ownid, :tagid, CURDATE(), NULL)");
        query.bindValue(":vehid", id);
        query.bindValue(":ownid", chosenOwnId);
        query.bindValue(":tagid", chosenTagId);
        if (!query.exec())
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
            query.exec("ROLLBACK");
            return;
        }
    }

    //Если ТС выбрано из базы, а данные владельца получены из полей ввода
    else if (chosenVehId != -1 && chosenOwnId == -1)
    {
        query.prepare("INSERT INTO `vehicle_owner`(`phone_number`, `owner_category_id`) VALUES (:number, :cat)");
        query.bindValue(":number", ui->lineEdit_telephone->text());
        query.bindValue(":cat", ui->comboBoxowner->currentIndex() + 1);
        if (!query.exec())
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
            query.exec("ROLLBACK");
            return;
        }
        query.exec("SELECT LAST_INSERT_ID()");
        query.next();
        int id = query.value(0).toInt();
        switch (ui->comboBoxowner->currentIndex())
        {
        case 0:
        {
            query.prepare("INSERT INTO `individual`(`owner_id`, `full_name`, `registration_adress`) VALUES (:id, :name, :adr)");
            query.bindValue(":id", id);
            query.bindValue(":name", ui->lineEdit_fullname->text());
            query.bindValue(":adr", ui->lineEdit_addr->text());
            if (!query.exec())
            {
                QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
                query.exec("ROLLBACK");
                return;
            }
            break;
        }
        case 1:
        {
            query.prepare("INSERT INTO `legal_entity`(`owner_id`, `company_name`, `legal_adress`, `physical_adress`, "
                          "`chief`) VALUES (:id, :name, :adr, :padr, :ch)");
            query.bindValue(":id", id);
            query.bindValue(":name", ui->lineEdit_fullname->text());
            query.bindValue(":adr", ui->lineEdit_addr->text());
            query.bindValue(":padr", ui->lineEdit_phaddr->text());
            query.bindValue(":ch", ui->lineEdit_chief->text());
            if (!query.exec())
            {
                QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
                query.exec("ROLLBACK");
                return;
            }
            break;
        }
        default:
            break;
        }
        query.prepare("INSERT INTO `vehicle_registration`(`vehicle_id`, `owner_id`, `tag_id`, `registration_date`, "
                      "`removal_date`) VALUES (:vehid, :ownid, :tagid, CURDATE(), NULL)");
        query.bindValue(":vehid", chosenVehId);
        query.bindValue(":ownid", id);
        query.bindValue(":tagid", chosenTagId);
        if (!query.exec())
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
            query.exec("ROLLBACK");
            return;
        }
    }

    //Если и ТС, и владелец вводятся через поля
    else if (chosenVehId == -1 && chosenOwnId == -1)
    {
        query.prepare("INSERT INTO `vehicle`(`brand_name`, `manufacture_year`, `engine_capacity_hp`, `VIN_code`, "
                      "`vehicle_colour`, `type_id`) VALUES (:name, :year, :hp, :vin, :col, :type)");
        query.bindValue(":name", ui->lineEdit_brand->text());
        query.bindValue(":year", ui->lineEdit_year->text().toInt());
        query.bindValue(":hp", ui->lineEdit_hp->text().toInt());
        query.bindValue(":vin", ui->lineEdit_vin->text());
        query.bindValue(":col", ui->lineEdit_color->text());
        query.bindValue(":type", ui->comboBoxtype->currentIndex() + 1);
        if (!query.exec())
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
            query.exec("ROLLBACK");
            return;
        }
        query.exec("SELECT LAST_INSERT_ID()");
        query.next();
        int idVeh = query.value(0).toInt();
        switch (ui->comboBoxtype->currentIndex())
        {
        case 0:
        {
            query.prepare("INSERT INTO `car_transport`(`vehicle_id`, `body_type`) VALUES (:id, :type)");
            query.bindValue(":id", idVeh);
            query.bindValue(":type", ui->lineEdit_extra->text());
            if (!query.exec())
            {
                QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
                query.exec("ROLLBACK");
                return;
            }
            break;
        }
        case 1:
        {
            query.prepare("INSERT INTO `freight_transport`(`vehicle_id`, `unloading_mode`) VALUES (:id, :mode)");
            query.bindValue(":id", idVeh);
            query.bindValue(":mode", ui->lineEdit_extra->text());
            if (!query.exec())
            {
                QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
                query.exec("ROLLBACK");
                return;
            }
            break;
        }
        case 2:
        {
            query.prepare("INSERT INTO `passenger_transport`(`vehicle_id`, `number_of_seats`) VALUES (:id, :number)");
            query.bindValue(":id", idVeh);
            query.bindValue(":number", ui->lineEdit_extra->text().toInt());
            if (!query.exec())
            {
                QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
                query.exec("ROLLBACK");
                return;
            }
            break;
        }
        default:
            break;
        }
        query.prepare("INSERT INTO `vehicle_owner`(`phone_number`, `owner_category_id`) VALUES (:number, :cat)");
        query.bindValue(":number", ui->lineEdit_telephone->text());
        query.bindValue(":cat", ui->comboBoxowner->currentIndex() + 1);
        if (!query.exec())
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
            query.exec("ROLLBACK");
            return;
        }
        query.exec("SELECT LAST_INSERT_ID()");
        query.next();
        int idOwn = query.value(0).toInt();
        switch (ui->comboBoxowner->currentIndex())
        {
        case 0:
        {
            query.prepare("INSERT INTO `individual`(`owner_id`, `full_name`, `registration_adress`) VALUES (:id, :name, :adr)");
            query.bindValue(":id", idOwn);
            query.bindValue(":name", ui->lineEdit_fullname->text());
            query.bindValue(":adr", ui->lineEdit_addr->text());
            if (!query.exec())
            {
                QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
                query.exec("ROLLBACK");
                return;
            }
            break;
        }
        case 1:
        {
            query.prepare("INSERT INTO `legal_entity`(`owner_id`, `company_name`, `legal_adress`, `physical_adress`, "
                          "`chief`) VALUES (:id, :name, :adr, :padr, :ch)");
            query.bindValue(":id", idOwn);
            query.bindValue(":name", ui->lineEdit_fullname->text());
            query.bindValue(":adr", ui->lineEdit_addr->text());
            query.bindValue(":padr", ui->lineEdit_phaddr->text());
            query.bindValue(":ch", ui->lineEdit_chief->text());
            if (!query.exec())
            {
                QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос на добавление");
                query.exec("ROLLBACK");
                return;
            }
            break;
        }
        default:
            break;
        }
        query.prepare("INSERT INTO `vehicle_registration`(`vehicle_id`, `owner_id`, `tag_id`, `registration_date`, "
                      "`removal_date`) VALUES (:vehid, :ownid, :tagid, CURDATE(), NULL)");
        query.bindValue(":vehid", idVeh);
        query.bindValue(":ownid", idOwn);
        query.bindValue(":tagid", chosenTagId);
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

//Метод проверки на наличие незаполненных полей
bool addRegDialog::checkfill()
{
    bool ok = true;
    m_list = findChildren<QLineEdit*>();
    for (int i = 0; i < m_list.size(); i++)
    {
        if (m_list.at(i)->text().isEmpty() && m_list.at(i)->isVisible())
            ok = false;
    }
    return ok;
}

//Слот обработки сигнала выбранного ТС из базы
void addRegDialog::receiveDataVeh(int ID)
{
    chosenVehId = ID;
    QSqlQuery query;
    query.prepare("SELECT `brand_name`, `manufacture_year`, `vin_code`, `engine_capacity_hp`, `vehicle_colour` "
                  "FROM `vehicle` WHERE :id = `vehicle_id`");
    query.bindValue(":id", chosenVehId);
    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить данные. Проверьте соединение с базой данных");
        return;
    }
    if (query.next())
    {
        ui->lineEdit_brand->setText(query.value(0).toString());
        ui->lineEdit_brand->setReadOnly(true);
        ui->lineEdit_year->setText(query.value(1).toString());
        ui->lineEdit_year->setReadOnly(true);
        ui->lineEdit_vin->setText(query.value(2).toString());
        ui->lineEdit_vin->setReadOnly(true);
        ui->lineEdit_hp->setText(query.value(3).toString());
        ui->lineEdit_hp->setReadOnly(true);
        ui->lineEdit_color->setText(query.value(4).toString());
        ui->lineEdit_color->setReadOnly(true);
        ui->lineEdit_extra->setVisible(false);
        ui->label_extra->setVisible(false);
        ui->comboBoxtype->setEnabled(false);
    }
}

//Слот обработки сигнала выбранного владельца из базы
void addRegDialog::receiveDataOwn(int ID)
{
    chosenOwnId = ID;
    QSqlQuery query;
    query.prepare("SELECT CASE `o`.`owner_category_id` WHEN 1 THEN `full_name` "
                  "ELSE `company_name` END as `owner`, CASE `o`.`owner_category_id` WHEN 1 THEN "
                  "`registration_adress` ELSE `legal_adress` END as `address`, `phone_number` FROM  "
                  "`vehicle_owner` as `o` LEFT JOIN `individual` as `i` ON `o`.`owner_id` = `i`.`owner_id` "
                  "LEFT JOIN `legal_entity` as `l` ON `o`.`owner_id`= `l`.`owner_id` WHERE `o`.`owner_id` = :id");
    query.bindValue(":id", chosenOwnId);
    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить данные. Проверьте соединение с базой данных");
        return;
    }
    if (query.next())
    {
        ui->lineEdit_fullname->setText(query.value(0).toString());
        ui->lineEdit_fullname->setReadOnly(true);
        ui->lineEdit_addr->setText(query.value(1).toString());
        ui->lineEdit_addr->setReadOnly(true);
        ui->label_addr->setText("Адрес");
        ui->lineEdit_telephone->setText(query.value(2).toString());
        ui->lineEdit_telephone->setReadOnly(true);
        ui->lineEdit_extra->setVisible(false);
        ui->label_extra->setVisible(false);
        ui->lineEdit_phaddr->setVisible(false);
        ui->label_phaddr->setVisible(false);
        ui->lineEdit_chief->setVisible(false);
        ui->label_chief->setVisible(false);
        ui->comboBoxowner->setEnabled(false);
    }
}

//Слот обработки сигнала выбранного номера из базы
void addRegDialog::receiveDataTag(int ID)
{
    chosenTagId = ID;
    QSqlQuery query;
    query.prepare("SELECT `vehicle_registration_tag` FROM `vehicle_registration_tag` WHERE `tag_id` = :id");
    query.bindValue(":id", chosenTagId);
    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить данные. Проверьте соединение с базой данных");
        return;
    }
    if (query.next())
    {
        ui->lineEdit_tag->setText(query.value(0).toString());
    }
}
