#include "infodlg.h"
#include "ui_infodlg.h"

InfoDlg::InfoDlg(QWidget *parent, int id) :
    QDialog(parent),
    ui(new Ui::InfoDlg)
{
    ui->setupUi(this);
    ID = id;
    this->getinfo();
}

InfoDlg::~InfoDlg()
{
    delete ui;
}

void InfoDlg::on_btn_ok_clicked()
{
    this->close();
}

void InfoDlg::getinfo()
{
    query.prepare("SELECT * FROM `road_accident` WHERE `accident_id` = :id");
    query.bindValue(":id", ID);
    query.exec();
    query.next();
    ui->label_cause->setText(ui->label_cause->text() + " " + query.value(4).toString());
    ui->label_cond->setText(ui->label_cond->text() + " " + query.value(1).toString());
    ui->label_date->setText(ui->label_date->text() + " " + query.value(0).toString());
    ui->label_desc->setText(ui->label_desc->text() + " " + query.value(3).toString());
    ui->label_time->setText(ui->label_time->text() + " " + query.value(2).toString());
    int placeID = query.value(8).toInt();
    query.prepare("SELECT * FROM `settlement` WHERE `place_id` = :placeid");
    query.bindValue(":placeid", placeID);
    query.exec();
    query.next();
    ui->label_address->setText(ui->label_address->text() + " " + query.value(1).toString() + ", " + query.value(2).toString());
    query.prepare("SELECT `participant_id` FROM `participation_in_accident` WHERE `accident_id` = :id");
    query.bindValue(":id", ID);
    query.exec();
    QVector<int> partIDs;
    while(query.next())
        partIDs.push_back(query.value(0).toInt());
    for (int i = 0; i < partIDs.size(); i++)
    {
        query.prepare("SELECT `participant_category_id` FROM `participant_of_accident` WHERE `participant_id` = :id");
        query.bindValue(":id", partIDs.at(i));
        query.exec();
        query.next();
        switch(query.value(0).toInt())
        {
        case 1:
        {
            query.prepare("SELECT * FROM `unregistered_vehicle` WHERE `participant_id` = :id");
            query.bindValue(":id", partIDs.at(i));
            query.exec();
            query.next();
            ui->label_veh->setText(ui->label_veh->text() + "\n" + query.value(2).toString() + ", гос. номер " +
                                   query.value(1).toString());
            break;
        }
        case 2:
        {
            query.prepare("SELECT `vehicle_id` FROM `registered_vehicle` WHERE `participant_id` = :id");
            query.bindValue(":id", partIDs.at(i));
            query.exec();
            query.next();
            int vehID = query.value(0).toInt();
            query.prepare("SELECT * FROM `vehicle` WHERE `vehicle_id` = :id");
            query.bindValue(":id", vehID);
            query.exec();
            query.next();
            QString brand = query.value(1).toString();
            query.prepare("SELECT `tag_id` FROM `vehicle_registration` WHERE `vehicle_id` = :id AND `removal_date` IS NULL");
            query.bindValue(":id", vehID);
            query.exec();
            query.next();
            int tagID = query.value(0).toInt();
            query.prepare("SELECT `vehicle_registration_tag` FROM `vehicle_registration_tag` WHERE `tag_id` = :id");
            query.bindValue(":id", tagID);
            query.exec();
            query.next();
            ui->label_veh->setText(ui->label_veh->text() + "\n" + brand + ", гос. номер " +
                                   query.value(0).toString());
            break;
        }
        case 3:
        {
            query.prepare("SELECT * FROM `pedestrian` WHERE `participant_id` = :id");
            query.bindValue(":id", partIDs.at(i));
            query.exec();
            query.next();
            ui->label_ped->setText(ui->label_ped->text() + "\n" + query.value(0).toString() + " " + query.value(1).toString() + " " +
                                   query.value(2).toString());
            break;
        }
        default:
            break;
        }
    } 
}
