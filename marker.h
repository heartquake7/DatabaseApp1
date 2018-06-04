#ifndef MARKER_H
#define MARKER_H

#include <QString>

class Marker
{
public:
    Marker(QString, QString, QString);
    QString getDate();
    QString getDesc();
    QString getAddress();
    void setDate(QString);
    void setDesc(QString);
    void setAddress(QString);

private:
    QString date;
    QString desc;
    QString address;
};

#endif // MARKER_H
