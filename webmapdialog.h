#ifndef WEBMAPDIALOG_H
#define WEBMAPDIALOG_H

#include <QDialog>
#include <QWebView>
#include <QSqlQuery>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkReply>
#include <QJsonArray>
#include "marker.h"

namespace Ui {
class WebMapDialog;
}

class WebMapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WebMapDialog(QWidget *parent = 0);
    ~WebMapDialog();

signals:
    readyShowMap();

private slots:
    void onResult(QNetworkReply* reply);
    void ShowMap();

    void on_btn_showfilter_clicked();

    void on_btn_applyfilter_clicked();

    void on_btn_resetfilter_clicked();

private:
    Ui::WebMapDialog *ui;
    QWebView* pWebView;
    QList<Marker*> listinfo;
    QStringList listaddr;
    QStringList listcoords;
    QJsonDocument document;
    QNetworkAccessManager* networkManager;
    void getAddr();
    void getCoords();
};

#endif // WEBMAPDIALOG_H
