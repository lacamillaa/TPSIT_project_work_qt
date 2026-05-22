#ifndef INTERFACEHANDLER_H
#define INTERFACEHANDLER_H

#include <QMainWindow>
#include <QJsonObject>
#include <QObject>
#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QStackedWidget>
#include <QStyle>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QPixmap>
#include <QJsonArray>

class InterfaceHandler
{
public:
    InterfaceHandler(QMainWindow* parent);
    void setup();
    void setPlayback(QJsonObject playback);
    void disconnect();

private:
    QMainWindow* parent;

    QStackedWidget* stackedWidget;
    QWidget* widget1;
    QWidget* displayBox;
    QLabel* copertina;
    QLabel* titolo;
    QLabel* artistaAlbum;
    QLabel* annoUscita;
    QLabel* currentTime;
    QSlider* progressBar;
    QLabel* totalTime;
    QLabel* messaggioErrore;
};

#endif // INTERFACEHANDLER_H
