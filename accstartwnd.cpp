#include "accstartwnd.h"
#include "ui_accstartwnd.h"

AccStartWnd::AccStartWnd(QWidget *parent, database* p_database, access acc) :
    QWidget(parent),
    ui(new Ui::AccStartWnd)
{
    ui->setupUi(this);
    ui->btn_resetfilter->setVisible(false);
    ui->dateEditfrom->setVisible(false);
    ui->dateEdit_to->setVisible(false);
    ui->label_datefrom->setVisible(false);
    ui->label_dateto->setVisible(false);
    db = p_database;
    Parent = parent;
    Acc = acc;

    // Проверка переменной, отвечающей за права доступа
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
    // Определение колонок модели таблицы
    this->setupModel(QStringList() << trUtf8("ID") << trUtf8("Дата ДТП") << trUtf8("Тип происшествия")
                     << trUtf8("Причина ДТП") << trUtf8("Место происшествия") << trUtf8("Описание ДТП"));
    this->createUI();

    //Присоединение сигнала изменения ширины ячейки к слоту изменения ширины строки
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionResized(int,int,int)),
                 this->ui->tableView, SLOT(resizeRowsToContents()));

}

AccStartWnd::~AccStartWnd()
{
    delete ui;
}

void AccStartWnd::on_btn_back_clicked()
{
    this->close();
}

void AccStartWnd::setupModel(const QStringList &headers)
{
    model = new QSqlQueryModel(this);

    // Запрос на выборку данных о ДТП
    query.exec("SELECT `Accident_ID`, `Accident_date`, `Type_of_accident`, `Cause_of_accident`, "
               "CONCAT(`street`, ', ', `house_number`) as `address`, `accident_description` FROM (SELECT `Accident_ID`, "
               "`Accident_date`, `Type_of_accident`, `Cause_of_accident`, `Place_ID`, `accident_description` FROM `road_accident` "
               "as `a`,  `type_of_accident` as `t` WHERE `t`.`type_of_accident_id` = `a`.`type_of_accident_id`) "
               "as `one` INNER JOIN (SELECT `s`.`place_id` as `place_id`, `street`, `house_number` FROM "
               "`place_of_accident` as `p` INNER JOIN `settlement` as `s` ON `p`.`place_id` = `s`.`place_id`) "
               "as `two` ON `one`.`place_id` = `two`.`place_id` ORDER BY `accident_date` DESC");
    model->setQuery(query);

    //Установка заголовков для таблицы
    for (int i = 0, j = 0; i < model->columnCount(); i++, j++)
    {
        model->setHeaderData(i, Qt::Horizontal, headers[j]);
    }
}

void AccStartWnd::createUI()
{
     ui->tableView->setModel(model);
     ui->tableView->setColumnHidden(0, true);
     ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
     ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
     ui->tableView->resizeColumnsToContents();
     ui->tableView->resizeRowsToContents();
     ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
     ui->tableView->horizontalHeader()->setStretchLastSection(true);
     ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
     ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

     //Если права доступа - администратор, то разрешить использование контекстного меню
     if (Acc == admin)
     {
        connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotgetInfo()));
        connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuReq(QPoint)));
     }
}

void AccStartWnd::slotCustomMenuReq(QPoint pos)
{
    switch (Acc)
    {
    case guest:
        break;

    //Создание элементов контекстного меню для прав доступа "Администратор"
    case admin:
    {
        QMenu* menu = new QMenu(this);
        QAction* getInfo = new QAction(trUtf8("Подробная информация"), this);
        QAction* deleteRec = new QAction(trUtf8("Удалить"), this);
        connect(getInfo, SIGNAL(triggered()), this, SLOT(slotgetInfo()));
        connect(deleteRec, SIGNAL(triggered()), this, SLOT(slotDeleteRecord()));
        menu->addAction(getInfo);
        menu->addAction(deleteRec);
        menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
        break;
    }
    default:
        break;
    }
}

void AccStartWnd::slotgetInfo()
{
    int row = ui->tableView->selectionModel()->currentIndex().row();
    if (row >= 0)
    {
           QModelIndex current = ui->tableView->currentIndex();
           int accid = current.sibling(current.row(), 0).data().toInt();
           InfoDlg* info = new InfoDlg(0, accid);
           info->show();
    }
   return;
}

void AccStartWnd::slotDeleteRecord()
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
           int accid = current.sibling(current.row(), 0).data().toInt();
           int placeID;
           QSqlQuery query;
           query.prepare("SELECT `place_id` FROM `road_accident` WHERE `accident_id` = :id");
           query.bindValue(":id", accid);
           query.exec();
           query.next();
           placeID = query.value(0).toInt();
           query.prepare("SELECT `participant_id` FROM `participation_in_accident` WHERE `accident_id` = :id");
           query.bindValue(":id", accid);
           query.exec();
           QVector<int> partIDs;
           while (query.next())
               partIDs.push_back(query.value(0).toInt());
           query.prepare("DELETE FROM `participation_in_accident` WHERE `accident_id` = :id");
           query.bindValue(":id", accid);
           if (!query.exec())
           {
               QMessageBox::critical(this, trUtf8("Внимание"), trUtf8("Не удалось удалить запись"));
               return;
           }
           for (int i = 0; i < partIDs.size(); i++)
           {        
               query.prepare("DELETE FROM `participant_of_accident` WHERE `participant_id` = :id");
               query.bindValue(":id", partIDs.at(i));
               if (!query.exec())
               {
                   QMessageBox::critical(this, trUtf8("Внимание"), trUtf8("Не удалось удалить запись"));
                   return;
               }
           }
           query.prepare("DELETE FROM `road_accident` WHERE `accident_id` = :id");
           query.bindValue(":id", accid);
           if (!query.exec())
           {
               QMessageBox::critical(this, trUtf8("Внимание"), trUtf8("Не удалось удалить запись"));
               return;
           }
           query.prepare("DELETE FROM `place_of_accident` WHERE `place_id` = :id");
           query.bindValue(":id", placeID);
           if (!query.exec())
           {
               QMessageBox::critical(this, trUtf8("Внимание"), trUtf8("Не удалось удалить запись"));
               return;
           }
           QMessageBox::information(this, trUtf8("Успешно"), trUtf8("Запись успешно удалена"));
           model->setQuery(model->query().lastQuery());
           ui->tableView->setCurrentIndex(model->index(-1, -1));
       }
    }
}

void AccStartWnd::oncloseDlg()
{
    model->setQuery(model->query().lastQuery());
}

//Слот обработки нажатия кнопки показа на карте, создание дочернего диалога класса WebMapDialog
void AccStartWnd::on_btn_map_clicked()
{
    WebMapDialog* webdlg = new WebMapDialog(this);
    webdlg->show();
}

void AccStartWnd::on_btn_showfilter_clicked()
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

void AccStartWnd::on_btn_applyfilter_clicked()
{
    model->setQuery(QString("SELECT * FROM (SELECT `Accident_ID`, `Accident_date`, `Type_of_accident`, `Cause_of_accident`, "
                            "CONCAT(`street`, ', ', `house_number`) as `address`, `accident_description` FROM (SELECT `Accident_ID`, "
                            "`Accident_date`, `Type_of_accident`, `Cause_of_accident`, `Place_ID`, `accident_description` FROM `road_accident` "
                            "as `a`,  `type_of_accident` as `t` WHERE `t`.`type_of_accident_id` = `a`.`type_of_accident_id`) "
                            "as `one` INNER JOIN (SELECT `s`.`place_id` as `place_id`, `street`, `house_number` FROM "
                            "`place_of_accident` as `p` INNER JOIN `settlement` as `s` ON `p`.`place_id` = `s`.`place_id`) "
                            "as `two` ON `one`.`place_id` = `two`.`place_id` "
                            "ORDER BY `accident_date` DESC) as `four` WHERE `four`.`accident_date` "
                            "BETWEEN '%1' AND '%2'").arg(ui->dateEditfrom->text(), ui->dateEdit_to->text()));
}

void AccStartWnd::on_btn_resetfilter_clicked()
{
    model->setQuery(query);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void AccStartWnd::on_btn_add_clicked()
{
    addAccDialog* adddlg = new addAccDialog(this);
    connect(adddlg, SIGNAL(closethis()), this, SLOT(oncloseDlg()));
    adddlg->show();
}
