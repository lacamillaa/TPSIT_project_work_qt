#include "mainwindow.h"

#include <QApplication>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QNetworkAccessManager m;
    QNetworkRequest req(QUrl("http://localhost:3000/auth"));
    QNetworkReply *reply = m.get(req);
    QByteArray resp = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(resp);
    QJsonObject obj = doc.object();
    qDebug() << "token: " << obj["access_token"].toString();
    w.show();
    return QCoreApplication::exec();
}
