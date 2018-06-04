#include "webmapdialog.h"
#include "ui_webmapdialog.h"

WebMapDialog::WebMapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebMapDialog)
{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager();
    //Присоединение сигнала выполнения запроса менеджера к слоту
    connect(networkManager, &QNetworkAccessManager::finished, this, &WebMapDialog::onResult);
    pWebView = new QWebView(this);
    pWebView->setGeometry(0, 40, 1000, 660);
    ui->btn_applyfilter->setVisible(false);
    ui->btn_resetfilter->setVisible(false);
    ui->dateEditfrom->setVisible(false);
    ui->dateEdit_to->setVisible(false);
    ui->label_datefrom->setVisible(false);
    ui->label_dateto->setVisible(false);
    connect(this, SIGNAL(readyShowMap()), this, SLOT(ShowMap()));
    this->getAddr();
    this->getCoords();
}

WebMapDialog::~WebMapDialog()
{
    for (int i = 0; i < listinfo.count(); i++)
        delete listinfo.at(i);
    delete ui;
}

void WebMapDialog::getAddr()
{
    listinfo.clear();
    listaddr.clear();
    QSqlQuery query;

    //Запрос на выборку всех адресов ДТП из базы
    if (!query.exec("SELECT CONCAT(`street`, ' ', `house_number`), `accident_date`, `accident_description`,  "
                    "CONCAT(`street`, ', ', `house_number`) "
                    "FROM `settlement` as `s`, `road_accident` as `r` WHERE "
                    "`s`.`place_id` = `r`.`place_id`"))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить адреса из базы");
        return;
    }
    while (query.next())
    {
        Marker* ptr = new Marker(query.value(1).toString(), query.value(2).toString(), query.value(3).toString());
        listinfo.append(ptr);
        listaddr << query.value(0).toString();
    }
}

//Получить координаты адреса через геокодер
void WebMapDialog::getCoords()
{
    listcoords.clear();
    QEventLoop loop;
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    for (int i = 0; i < listaddr.count(); i++)
    {
    networkManager->get(QNetworkRequest(QUrl("https://geocode-maps.yandex.ru/1.x/?format=json&geocode=Пенза "
                                             + listaddr.at(i))));
    loop.exec();
    }
}

//Если ответ геокодера получен, приступить к разбору JSON-документа
void WebMapDialog::onResult(QNetworkReply* reply)
{
   int a = reply->error();
   if(!reply->error())
   {
       document = QJsonDocument::fromJson(reply->readAll());
       QJsonObject root = document.object();
       QJsonValue jv = root.value("response");
       QJsonObject obj = jv.toObject();
       jv = obj.value("GeoObjectCollection");
       obj = jv.toObject();
       jv = obj.value("featureMember");
       QJsonArray ja = jv.toArray();
       obj = ja.at(0).toObject();
       jv = obj.value("GeoObject");
       obj = jv.toObject();
       jv = obj.value("Point");
       obj = jv.toObject();
       jv = obj.value("pos");
       listcoords << jv.toString();
       if (listaddr.count() == listcoords.count())
           emit readyShowMap();
   }
   else
   {
       QMessageBox::critical(this, "Ошибка", "Произошла ошибка при геокодировании. Код ошибки: " + QString::number(a));
   }
}

//Показать карту с маркерами на ней
void WebMapDialog::ShowMap()
{
    for (int i = 0; i < listcoords.count(); i++)
    {
        QStringList tmp = listcoords.at(i).split(" ");
        QStringList tmp2;
        tmp2 << tmp.at(1) << tmp.at(0);
        listcoords[i] = tmp2.join(", ");
    }
    QString head = "<!DOCTYPE html><html><head><script src='https://maps.api.2gis.ru/2.0/loader.js'></script></head>"
            "<body><div id='map' style='width: 100%; height: 640px;'></div>"
            "<script>DG.then(function() {var map = DG.map('map', {center: [53.2, 45.00],zoom: 11, "
                   "maxBounds: [[53.5, 45.3],[52.9, 44.7]], minZoom: 9, maxZoom: 17}); "
                   "var myIcon = DG.icon({iconUrl: 'http://adrive.by/img/rr/specsign/znak_avar_ost.png', "
                   "iconSize: [30, 30]}); ";
    QString tail = "});</script></body></html>";
    for (int i = 0; i < listcoords.count(); i++)
        head += ("DG.marker([" + listcoords.at(i) + "], {icon: myIcon}).addTo(map).bindPopup('"
                                                    + listinfo.at(i)->getDate() + "<br />" + listinfo.at(i)->getDesc()
                 + "<br />" + listinfo.at(i)->getAddress() + "'); ");
    head += tail;
    pWebView->setHtml(head);
}

void WebMapDialog::on_btn_showfilter_clicked()
{
    ui->btn_applyfilter->setVisible(true);
    ui->btn_resetfilter->setVisible(true);
    ui->btn_resetfilter->setEnabled(false);
    ui->dateEditfrom->setVisible(true);
    ui->dateEditfrom->setDisplayFormat("yyyy-MM-dd");
    ui->dateEdit_to->setVisible(true);
    ui->dateEdit_to->setDisplayFormat("yyyy-MM-dd");
    ui->label_datefrom->setVisible(true);
    ui->label_dateto->setVisible(true);
}

void WebMapDialog::on_btn_applyfilter_clicked()
{
    listinfo.clear();
    listaddr.clear();
    QSqlQuery query;
    bool isEmpty = true;

    //Запрос на выборку всех адресов ДТП из базы
    if (!query.exec(QString("SELECT * FROM (SELECT CONCAT(`street`, ' ', `house_number`), `accident_date`, `accident_description`,  "
                    "CONCAT(`street`, ', ', `house_number`) "
                    "FROM `settlement` as `s`, `road_accident` as `r` WHERE "
                    "`s`.`place_id` = `r`.`place_id`) as `one` WHERE `accident_date` BETWEEN '%1' AND "
                            "'%2'").arg(ui->dateEditfrom->text(), ui->dateEdit_to->text())))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить адреса из базы");
        return;
    }
    while (query.next())
    {
        isEmpty = false;
        Marker* ptr = new Marker(query.value(1).toString(), query.value(2).toString(), query.value(3).toString());
        listinfo.append(ptr);
        listaddr << query.value(0).toString();
    }
    if (isEmpty)
    {
        QMessageBox::warning(this, "Предупреждение", "Нет результатов, удовлетворящих условию фильтра");
        return;
    }
    ui->btn_resetfilter->setEnabled(true);
    this->getCoords();
}

void WebMapDialog::on_btn_resetfilter_clicked()
{
    this->getAddr();
    this->getCoords();
}
