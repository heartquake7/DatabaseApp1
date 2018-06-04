#include "mainwindow.h"
#include <QTextEdit>
#include <QString>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDate>
#include <QList>
#include <QByteArray>
#include <QVariant>
#include <QTextStream>
#include <QSqlRecord>


int STATUS=0;//global varieble for add status in insert
int WAY=2;
int PLACECATID=0;
int id_place=0;
int hijacking_id;
int VAR;
QString Brand_name;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    nmodel = new QSqlTableModel(this);

    QSqlQuery query;

    //nmodel->setTable("vehicle");
   // nmodel->select();
    //ui->tableView->setModel(nmodel);
    this->setupMainModel(QStringList() << trUtf8("Дата угона")<<trUtf8("Название машины")<<trUtf8("Статус угона")
                         <<trUtf8("Улица или название трассы")<<trUtf8("Номер дома или КМ трассы")<<trUtf8("номер автомобиля")<<trUtf8("Цвет авто"));



        ui->tableView->setModel(modelMain);     // Устанавливаем модель на TableView
        //ui->tableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
        // Разрешаем выделение строк
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        // Устанавливаем режим выделения лишь одно строки в таблице
        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        // Устанавливаем размер колонок по содержимому
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->horizontalHeader()->setStretchLastSection(true);

   //connect(FindButton,SIGNAL(clicked()),this,SLOT(open_form2()));
}
/*
 * "a.Hijacking_date,""b.Brand_name,""a.Vehicle_ID,""b.Vehicle_ID," "FROM" "vehicle_hijacking as a"",vehicle as b" "WHERE a.Vehicle_ID=b.Vehicle_ID
 */
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow:: setupMainModel(const QStringList &headers)
{
    /* Производим инициализацию модели представления данных
         * */
        modelMain = new QSqlQueryModel(this);


        modelMain->setQuery("SELECT "
                            "a.Hijacking_date "
                            "," "b.Brand_name "
                            "," "c.Status_name ""  ,"
                            "CASE p.place_category_id WHEN 1 THEN s.street "
                            "ELSE Route_name END "", "
                            "CASE p.place_category_id WHEN 1 THEN s.house_number ELSE sr.route_km END "
                            "FROM "
                            "vehicle_hijacking as a "","
                            "vehicle as b "","
                            "vehicle_status as c "","
                            "place_of_accident as p LEFT JOIN settlement as s "
                            "ON p.place_id = s.place_id LEFT JOIN section_of_the_route as sr ON p.place_id = sr.place_id "
                            "WHERE ""a.Vehicle_ID ""= "
                            "b.Vehicle_ID "
                            "AND "
                            "a.Status_ID = "
                            "c.Status_ID"
                            " AND "
                           "p.place_id = a.place_id"
                            );

        /* Устанавливаем названия колонок в таблице с сортировкой данных
         * */
        for(int i = 0, j = 0; i < modelMain->columnCount(); i++, j++){
            modelMain->setHeaderData(i,Qt::Horizontal,headers[j]);
        }
}



void MainWindow::on_action_triggered()
{

        WindowOfAdd *form= new WindowOfAdd;
        form->setupUi(form);
        form->show();
        form->activateWindow();

}
void WindowOfAdd::on_comboBox_activated(int arg1)
{

    switch (arg1) {
    case 1:
      {

        STATUS=1;
        break;
      }
    case 2:
       {

        STATUS=2;
    break;
    }
    case 3:
    {

        STATUS=3;
        break;
    }
    default:
        break;
    }
}
void WindowOfAdd::on_comboBox_2_activated(int arg1)
{
    switch (arg1) {
    case 1:
      {

        PLACECATID=1;
        break;
      }
    case 2:
       {
        PLACECATID=2;
        break;
    }
    default:
        break;
    }
}
void WindowOfAdd::on_pushButton_clicked()
{
    QSqlDatabase nDatabase1;
    nDatabase1 = QSqlDatabase::addDatabase("QMYSQL");
    nDatabase1.setDatabaseName("norm_gibdd");
    nDatabase1.setHostName("127.0.0.1");
    nDatabase1.setPort(3306);
    nDatabase1.setUserName("root");
    if(!nDatabase1.open())
    {
        QMessageBox::critical(this,"Error",nDatabase1.lastError().text());
        return;
    }

    int id_vehicle=6;

    QString date1 = WindowOfAdd::textEdit->toPlainText(); //считываем улицу
    QString street = WindowOfAdd::textEdit_2->toPlainText(); //считываем номер
    QString number = WindowOfAdd::textEdit_3->toPlainText(); //считываем название машины
    //id_vehicle = WindowOfAdd::textEdit_4->toPlainText().toInt(0,10); //считываем статус
    //QString date = WindowOfAdd::textEdit_5->toPlainText(); //считываем дату
    //вставляем
    /*
     *INSERT INTO `vehicle_hijacking` (`hijacking_date`, `hijacking_way_id`, `status_id`, `vehicle_id`, `place_id`, `status_date`) VALUES ("2016-08-12", 1, 2, 13, 19, NOW())
     */
    ScanCash();
    QSqlQuery query2;
    QSqlQuery query3;
    QSqlQuery query4;


    if(PLACECATID==1)
    {
        query2.prepare("INSERT INTO place_of_accident (Place_category_id) VALUES (?)");
        query2.addBindValue(PLACECATID);
        id_place++;
        WriteCash();
        if(!query2.exec())
        {
            QMessageBox::critical(this,"Error","Don't add id");
            id_place--;
            return;
        }
        query3.prepare("INSERT INTO settlement (Place_ID,Street,House_number) VALUES(?,?,?)");
        query3.addBindValue(id_place);
        query3.addBindValue(street);
        query3.addBindValue(number);
        if(!query3.exec())
        {
            QMessageBox::critical(this,"Error","Данные не добавлены где-то ошибка");
            return;
        }
    }
    if(PLACECATID==2)
    {
        query2.prepare("INSERT INTO place_of_accident (Place_category_id) VALUES (?)");
        query2.addBindValue(PLACECATID);
        id_place++;
        WriteCash();
        if(!query2.exec())
        {
            QMessageBox::critical(this,"Error","Don't add id");
            id_place--;
            return;
        }
        query4.prepare("INSERT INTO section_of_the_route (Place_ID,Route_name,Route_km) VALUES(?, ?, ?)");
        query4.addBindValue(id_place);
        query4.addBindValue(street);
        query4.addBindValue(number);
        if(!query4.exec())
        {
            QMessageBox::critical(this,"Error","Данные не добавлены где-то ошибка");
            return;
        }
    }
    QDate date;
    date = QDate::fromString(date1,"yyyy.MM.dd");
    QSqlQuery query1;

    /*query1.exec("INSERT INTO vehicle_hijacking (hijacking_id,"
                "hijacking_date,hijacking_way_id,status_id,vehicle_id,place_id,status_date) VALUE(10,2013-22-11,1,2,13,19,NOW() )");*/

    query1.prepare("INSERT INTO vehicle_hijacking (hijacking_date, hijacking_way_id, status_id,vehicle_id,place_id,status_date) ""VALUES (?, ?, ?, ?, ?, NOW())");
    query1.addBindValue(date1);
    query1.addBindValue(WAY);
    query1.addBindValue(STATUS);
    query1.addBindValue(VAR);
    query1.addBindValue(id_place);
    if(!query1.exec())
    {
        QMessageBox::critical(this,"Error",nDatabase1.lastError().text());
        return;
    }
}

void MainWindow::on_action_6_triggered()
{
    addin_vehicle_hijacking *form1= new addin_vehicle_hijacking;
    form1->setupUi(form1);
    form1->show();
    form1->activateWindow();

}

void addin_vehicle_hijacking::on_pushButton_clicked()
{
    QSqlDatabase nDatabase2;
    nDatabase2 = QSqlDatabase::addDatabase("QMYSQL");
    nDatabase2.setDatabaseName("norm_gibdd");
    nDatabase2.setHostName("127.0.0.1");
    nDatabase2.setPort(3306);
    nDatabase2.setUserName("root1");
    if(!nDatabase2.open())
    {
        QMessageBox::critical(this,"Error",nDatabase2.lastError().text());
        return;
    }

    nmodel1 = new QSqlTableModel(this);
    nmodel1->setTable("vehicle_hijacking");
    nmodel1->select();
    addin_vehicle_hijacking::tableView1->setModel(nmodel1);
}

void addin_vehicle_hijacking::on_pushButton_2_clicked()
{
nmodel1->insertRow(nmodel1->rowCount());
}

void MainWindow::on_action_9_triggered()
{
    nDatabase = QSqlDatabase::addDatabase("QMYSQL");
    nDatabase.setDatabaseName("norm_gibdd");
    nDatabase.setHostName("127.0.0.1");
    nDatabase.setPort(3306);
    nDatabase.setUserName("root");
    if(!nDatabase.open())
    {
        QMessageBox::critical(this,"Error",nDatabase.lastError().text());
        return;
    }
    nmodel = new QSqlTableModel(this);

    QSqlQuery query;

    //nmodel->setTable("vehicle");
   // nmodel->select();
    //ui->tableView->setModel(nmodel);
    this->setupMainModel(QStringList() << trUtf8("Дата угона")<<trUtf8("Название машины")<<trUtf8("Статус угона")
                         <<trUtf8("Улица или название трассы")<<trUtf8("Номер дома или КМ трассы")<<trUtf8("номер автомобиля")<<trUtf8("Цвет авто"));



        ui->tableView->setModel(modelMain);     // Устанавливаем модель на TableView
        //ui->tableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
        // Разрешаем выделение строк
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        // Устанавливаем режим выделения лишь одно строки в таблице
        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        // Устанавливаем размер колонок по содержимому
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->horizontalHeader()->setStretchLastSection(true);
}



void addin_vehicle_hijacking::on_pushButton_3_clicked()
{
    nmodel1->removeRow(addin_vehicle_hijacking::tableView1->currentIndex().row());
}

void WindowOfAdd::WriteCash()
{
    QFile base("D:/Temp/Cash.txt");
    if (!base.open(QIODevice::WriteOnly | QIODevice::Text)) {

                return;
        }
    QTextStream outstream(&base);
    outstream << id_place;
}

void WindowOfAdd::ScanCash()
{
    QFile base("D:/Temp/Cash.txt");
    if (!base.open(QIODevice::ReadOnly | QIODevice::Text)) {

                return;
        }
    id_place=base.readLine().toInt();
}

void WindowOfAdd::on_pushButton_2_clicked()
{

    Information *form4= new Information;
    form4->setupUi(form4);
    form4->show();
    form4->activateWindow();
    WindowOfAdd::close();
}


void Information::on_pushButton_clicked()
{
         QSqlQuery query4;
         QSqlQueryModel *nmodel1;
         nmodel1 = new QSqlTableModel(this);
         //modelMain1 = new QSqlQueryModel(this);
         nmodel1->setQuery("SELECT a.Vehicle_ID as 'Идентификатор', a.Brand_name as 'Название машины', "
                           "a.VIN_code as 'ВИН код' FROM vehicle as a");
         Information::tableView->setModel(nmodel1);

}

void MainWindow::on_action_2_triggered()
{
    WindowOfUsingAuto *form5= new WindowOfUsingAuto ;
    form5->setupUi(form5);
    form5->show();
    form5->activateWindow();
}

void WindowOfUsingAuto::on_pushButton_clicked()
{
    //QSqlQuery query4;
    QSqlQueryModel *nmodel2;
    nmodel2 = new QSqlTableModel(this);
    //modelMain1 = new QSqlQueryModel(this);
    /*nmodel2->setQuery("SELECT a.Vehicle_ID as 'Идентификатор', a.Brand_name as 'Название машины', "
                      "a.VIN_code as 'ВИН код' FROM vehicle as a");*/
    nmodel2->setQuery("SELECT  `Brand_name` AS  `Транспортное средство` , COUNT(  `h`.`vehicle_id` ) AS  `Количество угонов` FROM  `vehicle` AS  `v` LEFT JOIN  `vehicle_hijacking` AS  `h` ON  `v`.`vehicle_id` =  `h`.`vehicle_id` GROUP BY  `Транспортное средство` ORDER BY  `Количество угонов` DESC  ");

    WindowOfUsingAuto::tableView->setModel(nmodel2);
}

void MainWindow::on_action_3_triggered()
{
    windowOfVehicle *form5= new windowOfVehicle ;
    form5->setupUi(form5);
    form5->show();
    form5->activateWindow();
}

void windowOfVehicle::on_pushButton_clicked()
{
    QSqlQuery query4;
    QSqlQueryModel *nmodel1;
    nmodel1 = new QSqlTableModel(this);
    //modelMain1 = new QSqlQueryModel(this);
    nmodel1->setQuery("SELECT  `Street` AS  `Улица` , COUNT( * ) AS  `Количество угонов` "
                      "FROM  `settlement` AS  `s`, "
                      "`vehicle_hijacking` AS  `h` WHERE "
                      "( `s`.`place_id` =  `h`.`place_id` )"
                      " GROUP BY  `Улица`");
    windowOfVehicle::tableView->setModel(nmodel1);
}

void MainWindow::on_action_4_triggered()
{
    almostway *form5= new almostway ;
    form5->setupUi(form5);
    form5->show();
    form5->activateWindow();
}

void almostway::on_pushButton_clicked()
{
    QSqlQueryModel *nmodel1;
    nmodel1 = new QSqlTableModel(this);
    //modelMain1 = new QSqlQueryModel(this);
    nmodel1->setQuery("SELECT  `way_name` AS  `Способ угона` , COUNT(  `hijacking_id` ) AS  `Количество случаев` FROM  `way_of_hijacking` AS  `w` LEFT JOIN  `vehicle_hijacking` AS  `h` ON  `w`.`hijacking_way_id` =  `h`.`hijacking_way_id` GROUP BY  `Способ угона` ORDER BY  `Количество случаев` DESC");
    almostway::tableView->setModel(nmodel1);
}

void WindowOfAdd::on_pushButton_3_clicked()
{
//удаляем place of accident
    QSqlQuery query;
    if(PLACECATID==1)
    {
        QString temp=QString::number(id_place);
        query.exec("DELETE FROM settlement WHERE Place_ID = "+temp);
        query.exec("DELETE FROM place_of_accident WHERE Place_ID = "+temp);

    }
    if(PLACECATID==2)
    {
        QString temp=QString::number(id_place);
        query.exec("DELETE FROM section_of_the_route WHERE Place_ID = "+temp);
        query.exec("DELETE FROM place_of_accident WHERE Place_ID = "+temp);
    }
    //удаляем vehicle_hijacking
    QString temp=QString::number(id_place);
    query.exec("DELETE FROM vehicle_hijacking WHERE Place_ID = "+temp);
}

void MainWindow::on_action_7_triggered()
{
    tablevehicle *form5= new tablevehicle ;
    form5->setupUi(form5);
    form5->show();
    form5->activateWindow();
    //if(form5->close())
}

void tablevehicle::on_pushButton_clicked()
{
    QSqlDatabase nDatabase2;
    nDatabase2 = QSqlDatabase::addDatabase("QMYSQL");
    nDatabase2.setDatabaseName("norm_gibdd");
    nDatabase2.setHostName("127.0.0.1");
    nDatabase2.setPort(3306);
    nDatabase2.setUserName("root1");
    if(!nDatabase2.open())
    {
        QMessageBox::critical(this,"Error",nDatabase2.lastError().text());
        return;
    }

    nmodel2 = new QSqlTableModel(this);
    nmodel2->setTable("vehicle");
    nmodel2->select();
    tablevehicle::tableView->setModel(nmodel2);
}

void tablevehicle::on_pushButton_2_clicked()
{
nmodel2->insertRow(nmodel2->rowCount());
}

void tablevehicle::on_pushButton_3_clicked()
{
    nmodel2->removeRow(tablevehicle::tableView->currentIndex().row());
}

void WindowOfUsingAuto::on_WindowOfUsingAuto_accepted()
{
    WindowOfUsingAuto::pushButton->click();
}

void Information::on_tableView_doubleClicked(const QModelIndex &index)
{
    QVariant a;
    a=Information::tableView->currentIndex().data();
    VAR=a.toInt();
    QSqlQuery query;
    QString tmp = QString::number(VAR);
    query.exec("SELECT Brand_name FROM vehicle WHERE vehicle_id= "+tmp);
    if(query.isSelect())
    {

        while (query.next())
        Brand_name = query.value(0).toString();
    }
    WindowOfAdd *form= new WindowOfAdd;
    form->setupUi(form);
    form->show();
    form->activateWindow();
    form->label_5->setText(Brand_name);
    Information::close();
}



void MainWindow::on_pushButton_clicked()
{
  QStringList headers;
    headers=QStringList() << trUtf8("Дата угона")<<trUtf8("Название машины")<<trUtf8("Статус угона")<<trUtf8("Улица или название трассы")<<trUtf8("Номер дома или КМ трассы")<<trUtf8("номер автомобиля")<<trUtf8("Цвет авто");

QString Finder;
    Finder = ui->textEdit->toPlainText();
    modelMain = new QSqlQueryModel(this);
    modelMain->setQuery("SELECT "
                        "a.Hijacking_date as `Дата угона`"
                        "," "b.Brand_name "
                        "," "c.Status_name ""  ,"
                        "CASE p.place_category_id WHEN 1 THEN s.street "
                        "ELSE Route_name END "", "
                        "CASE p.place_category_id WHEN 1 THEN s.house_number ELSE sr.route_km END "
                        "FROM "
                        "vehicle_hijacking as a "","
                        "vehicle as b "","
                        "vehicle_status as c "","
                        "place_of_accident as p LEFT JOIN settlement as s "
                        "ON p.place_id = s.place_id LEFT JOIN section_of_the_route as sr ON p.place_id = sr.place_id "
                        "WHERE ""a.Vehicle_ID ""= "
                        "b.Vehicle_ID "
                        "AND "
                        "a.Status_ID = "
                        "c.Status_ID"
                        " AND "
                       "p.place_id = a.place_id "
                        "AND "
                       "b.Brand_name LIKE ""'"+Finder+"'"
                        );
    for(int i = 0, j = 0; i < modelMain->columnCount(); i++, j++){
        modelMain->setHeaderData(i,Qt::Horizontal,headers[j]);
    ui->tableView->setModel(modelMain);

    }
}

