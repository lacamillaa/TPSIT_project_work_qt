#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMainWindow>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QNetworkAccessManager m;
    char APIurl[] = "http://localhost:3000/auth";
    QNetworkRequest req((QUrl(APIurl)));
    QNetworkReply *reply = m.get(req);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QString resp = reply->readAll();
        qDebug() << resp;
    }
    else {
        qDebug() << "Errore: " << reply->errorString();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
