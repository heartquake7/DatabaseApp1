#include "startwindow.h"
#include "database.h"
#include <QApplication>
#include <QTranslator>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    qApp->setStyle(QStyleFactory::create("fusion"));
    database mydb("gibdd", "127.0.0.1", 3306, "root", "");
    if (!mydb.open())
        exit(0);
    StartWindow* strtwnd = new StartWindow(0, &mydb);
    strtwnd->show();
    return app.exec();
}
