#include "marker.h"

Marker::Marker(QString dt, QString dsc, QString addr)
{
    date = dt;
    desc = dsc;
    address = addr;
}

QString Marker::getDate()
{
   return date;
}

QString Marker::getDesc()
{
   return desc;
}

QString Marker::getAddress()
{
   return address;
}

void Marker::setDate(QString dt)
{
   date = dt;
}

void Marker::setDesc(QString dsc)
{
   desc = dsc;
}

void Marker::setAddress(QString addr)
{
   address = addr;
}
