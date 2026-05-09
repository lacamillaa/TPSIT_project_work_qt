#include "mainwindow.h"
#include "SpotifyAuthenticator.h"

#include <ctime>

#include <QApplication>
#include <QPushButton>
#include <QDesktopServices>
#include <QVBoxLayout>
#include <QUrl>
#include <QTcpServer>
#include <QHostAddress>
#include <QFile>
#include <QTextStream>
#include <QUrlQuery>

int main(int argc, char *argv[]) {
    srand(NULL);

    QFile file("credentials.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "ERRORE: impossibile aprire il file";
    }
    QTextStream input(&file);
    QString client_id = input.readLine().trimmed().split("=")[1];
    QString client_secret = input.readLine().trimmed().split("=")[1];
    QString redirect_uri = input.readLine().trimmed().split("=")[1];
    file.close();

    QApplication app(argc, argv);
    MainWindow window;

    SpotifyAuthenticator auth = SpotifyAuthenticator(client_id, client_secret, redirect_uri);
    auth.startListening();
    auth.makeAuthCall();

    window.show();
    return app.exec();
}
