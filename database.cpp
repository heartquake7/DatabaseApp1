#include "database.h"

database::database(QString dbname, QString ip, int port, QString username, QString password)
{
    setaccess(guest);
    nDatabase = QSqlDatabase::addDatabase("QMYSQL");
    nDatabase.setDatabaseName(dbname);
    nDatabase.setHostName(ip);
    nDatabase.setPort(port);
    nDatabase.setUserName(username);
    nDatabase.setPassword(password);
}

database::~database()
{
    nDatabase.close();
    QSqlDatabase::removeDatabase(nDatabase.connectionName());
}

bool database::open()
{
    if(!nDatabase.open())
    {
        QMessageBox::critical(0, "Ошибка", nDatabase.lastError().text());
        return false;
    }
    else
        return true;
}

//Установить права доступа
void database::setaccess(access Acc)
{
    acc = Acc;
    return;
}

//Получить права доступа
access database::getaccess()
{
    return acc;
}

//Метод авторизации
bool database::auth(QString login, QString pwd)
{
    QSqlQuery query;
    query.prepare("SELECT * from users WHERE :login = login");
    query.bindValue(":login", login);
    if (!query.exec())
    {
        QMessageBox::critical(0, "Ошибка", "Не удалось выполнить запрос");
        return false;
    }
    if (!query.next())
    {
        QMessageBox::critical(0, "Ошибка", "Несуществующий пользователь");
        return false;
    }
    else
    {
        // Пароль зашифрован md5
        QString res = QCryptographicHash::hash(pwd.toUtf8(), QCryptographicHash::Md5).toHex();
        res = QCryptographicHash::hash(res.toUtf8(), QCryptographicHash::Md5).toHex();
        query.prepare("SELECT * from users WHERE :pwd = password");
        query.bindValue(":pwd", res);
        if (!query.exec())
        {
            QMessageBox::critical(0, "Ошибка", "Не удалось выполнить запрос");
            return false;
        }
        if (!query.next())
        {
            QMessageBox::critical(0, "Ошибка", "Неверный пароль");
            return false;
        }
        else
        {
            QMessageBox::information(0, "Вход выполнен", "Вы успешно вошли в учетную запись");
            return true;
        }
    }
}
