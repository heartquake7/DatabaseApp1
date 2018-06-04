#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QVariant>

enum access {admin, guest};

class database : public QObject
{
    Q_OBJECT
public:
    explicit database(QString, QString, int, QString, QString);
    ~database();
    void setaccess(access);
    access getaccess();
    bool open();
    bool auth(QString login, QString pwd);

private:
    QSqlDatabase nDatabase;
    access acc;

signals:

public slots:
};

#endif // DATABASE_H
