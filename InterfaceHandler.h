#ifndef INTERFACEHANDLER_H
#define INTERFACEHANDLER_H

#include <QMainWindow>
#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QFontMetrics>
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
    void setError(QString errorString);

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
