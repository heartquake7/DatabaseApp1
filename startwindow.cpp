#include "startwindow.h"
#include "ui_startwindow.h"

StartWindow::StartWindow(QWidget *parent, database* p_database) :
    QDialog(parent),
    ui(new Ui::StartWindow)
{
    ui->setupUi(this);
    db = p_database;
    QPalette plt = this->palette();
   // plt.setColor(QPalette::Window, QColor(175, 238, 238));
    this->setAutoFillBackground(true);
    this->setPalette(plt);

    //Ограничения на кол-во символов для полей ввода
    ui->lineEdit_login->setMaxLength(20);
    ui->lineEdit_pwd->setMaxLength(32);

    //Установка недопустимого набора символов для поля имени пользователя
    ui->lineEdit_login->setValidator(new QRegExpValidator(QRegExp("[^!@%^_*&$#]+"), this));

    //Скрыть ненужные в данный момент элементы интерфейса
    ui->btn_back->setVisible(false);
    ui->pushButton1->setVisible(false);
    ui->pushButton2->setVisible(false);
    ui->lineEdit_login->setVisible(false);
    ui->label_login->setVisible(false);
    ui->label_pwd->setVisible(false);
    ui->btn_submit->setVisible(false);
    ui->lineEdit_pwd->setEchoMode(QLineEdit::Password);
    ui->lineEdit_pwd->setVisible(false);
    ui->label_acc->setVisible(false);
}

StartWindow::~StartWindow()
{
    delete ui;
}

//Если выбран режим гостя
void StartWindow::on_btn_guest_clicked()
{
    db->setaccess(guest);
    ui->label_hello->setVisible(false);
    ui->btn_guest->setVisible(false);
    ui->btn_login->setVisible(false);
    ui->lineEdit_login->setVisible(false);
    ui->label_login->setVisible(false);
    ui->label_pwd->setVisible(false);
    ui->btn_submit->setVisible(false);
    ui->lineEdit_pwd->setVisible(false);
    QGraphicsOpacityEffect* effect1 = new QGraphicsOpacityEffect(ui->pushButton1);
    QGraphicsOpacityEffect* effect2 = new QGraphicsOpacityEffect(ui->pushButton2);
    ui->pushButton1->setGraphicsEffect(effect1);
    ui->pushButton2->setGraphicsEffect(effect2);
    QPropertyAnimation* animation1 = new QPropertyAnimation(effect1, "opacity");
    animation1->setEasingCurve(QEasingCurve::InCirc);
    animation1->setDuration(700);
    animation1->setStartValue(0.01);
    animation1->setEndValue(1.0);
    animation1->start(QPropertyAnimation::DeleteWhenStopped);
    QPropertyAnimation* animation2 = new QPropertyAnimation(effect2, "opacity");
    animation2->setEasingCurve(QEasingCurve::InCirc);
    animation2->setDuration(700);
    animation2->setStartValue(0.01);
    animation2->setEndValue(1.0);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);
    ui->btn_back->setVisible(true);
    ui->pushButton1->setVisible(true);
    ui->pushButton2->setVisible(true);
    ui->label_acc->setText(ui->label_acc->text() + "Гость");
    ui->label_acc->setVisible(true);

}

//Если запрошена авторизация
void StartWindow::on_btn_login_clicked()
{
    QGraphicsOpacityEffect* effect1 = new QGraphicsOpacityEffect(ui->lineEdit_login);
    QGraphicsOpacityEffect* effect2 = new QGraphicsOpacityEffect(ui->lineEdit_pwd);
    QGraphicsOpacityEffect* effect3 = new QGraphicsOpacityEffect(ui->label_login);
    QGraphicsOpacityEffect* effect4 = new QGraphicsOpacityEffect(ui->label_pwd);
    QGraphicsOpacityEffect* effect5 = new QGraphicsOpacityEffect(ui->btn_submit);
    ui->lineEdit_login->setGraphicsEffect(effect1);
    ui->lineEdit_pwd->setGraphicsEffect(effect2);
    ui->label_login->setGraphicsEffect(effect3);
    ui->label_pwd->setGraphicsEffect(effect4);
    ui->btn_submit->setGraphicsEffect(effect5);
    QPropertyAnimation* animation1 = new QPropertyAnimation(effect1, "opacity");
    animation1->setEasingCurve(QEasingCurve::InCirc);
    animation1->setDuration(700);
    animation1->setStartValue(0.01);
    animation1->setEndValue(1.0);
    animation1->start(QPropertyAnimation::DeleteWhenStopped);
    QPropertyAnimation* animation2 = new QPropertyAnimation(effect2, "opacity");
    animation2->setEasingCurve(QEasingCurve::InCirc);
    animation2->setDuration(700);
    animation2->setStartValue(0.01);
    animation2->setEndValue(1.0);
    animation2->start(QPropertyAnimation::DeleteWhenStopped);
    QPropertyAnimation* animation3 = new QPropertyAnimation(effect3, "opacity");
    animation3->setEasingCurve(QEasingCurve::InCirc);
    animation3->setDuration(700);
    animation3->setStartValue(0.01);
    animation3->setEndValue(1.0);
    animation3->start(QPropertyAnimation::DeleteWhenStopped);
    QPropertyAnimation* animation4 = new QPropertyAnimation(effect4, "opacity");
    animation4->setEasingCurve(QEasingCurve::InCirc);
    animation4->setDuration(700);
    animation4->setStartValue(0.01);
    animation4->setEndValue(1.0);
    animation4->start(QPropertyAnimation::DeleteWhenStopped);
    QPropertyAnimation* animation5 = new QPropertyAnimation(effect5, "opacity");
    animation5->setEasingCurve(QEasingCurve::InCirc);
    animation5->setDuration(700);
    animation5->setStartValue(0.01);
    animation5->setEndValue(1.0);
    animation5->start(QPropertyAnimation::DeleteWhenStopped);
    ui->lineEdit_login->setVisible(true);
    ui->label_login->setVisible(true);
    ui->label_pwd->setVisible(true);
    ui->btn_submit->setVisible(true);
    ui->lineEdit_pwd->setVisible(true);
}

//Попытка авторизации после ввода логина и пароля
void StartWindow::on_btn_submit_clicked()
{
    bool ok = false;
    QString login = ui->lineEdit_login->text();
    QString password = ui->lineEdit_pwd->text();
    ui->lineEdit_pwd->clear();
    ok = db->auth(login, password);
    if (!ok)
        return;
    else
    {
      db->setaccess(admin);
      QGraphicsOpacityEffect* effect1 = new QGraphicsOpacityEffect(ui->pushButton1);
      QGraphicsOpacityEffect* effect2 = new QGraphicsOpacityEffect(ui->pushButton2);
      ui->pushButton1->setGraphicsEffect(effect1);
      ui->pushButton2->setGraphicsEffect(effect2);
      QPropertyAnimation* animation1 = new QPropertyAnimation(effect1, "opacity");
      animation1->setEasingCurve(QEasingCurve::InCirc);
      animation1->setDuration(700);
      animation1->setStartValue(0.01);
      animation1->setEndValue(1.0);
      animation1->start(QPropertyAnimation::DeleteWhenStopped);
      QPropertyAnimation* animation2 = new QPropertyAnimation(effect2, "opacity");
      animation2->setEasingCurve(QEasingCurve::InCirc);
      animation2->setDuration(700);
      animation2->setStartValue(0.01);
      animation2->setEndValue(1.0);
      animation2->start(QPropertyAnimation::DeleteWhenStopped);
      ui->lineEdit_login->setVisible(false);
      ui->label_login->setVisible(false);
      ui->label_pwd->setVisible(false);
      ui->btn_submit->setVisible(false);
      ui->lineEdit_pwd->setVisible(false);
      ui->label_hello->setVisible(false);
      ui->btn_guest->setVisible(false);
      ui->btn_login->setVisible(false);
      ui->btn_back->setVisible(true);
      ui->pushButton1->setVisible(true);
      ui->pushButton2->setVisible(true);
      ui->label_acc->setText(ui->label_acc->text() + "Администратор");
      ui->label_acc->setVisible(true);
    }
}

//Создать и отобразить окно регистрации
void StartWindow::on_pushButton1_clicked()
{
    RegStartWnd* regstartwnd = new RegStartWnd(this, db, db->getaccess());
    regstartwnd->show();
}

//Вернуться назад
void StartWindow::on_btn_back_clicked()
{
   ui->btn_back->setVisible(false);
   ui->pushButton1->setVisible(false);
   ui->pushButton2->setVisible(false);
   ui->lineEdit_login->setVisible(false);
   ui->label_login->setVisible(false);
   ui->label_pwd->setVisible(false);
   ui->btn_submit->setVisible(false);
   ui->lineEdit_pwd->setVisible(false);
   ui->label_acc->setVisible(false);
   ui->label_hello->setVisible(true);
   ui->btn_login->setVisible(true);
   ui->btn_guest->setVisible(true);
   ui->label_acc->setText("Пользователь: ");
}

//Создать и отобразить окно ДТП
void StartWindow::on_pushButton2_clicked()
{
    AccStartWnd* accstartwnd = new AccStartWnd(this, db, db->getaccess());
    accstartwnd->show();
}
