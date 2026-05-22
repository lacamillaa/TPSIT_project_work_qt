#ifndef INTERFACEHANDLER_H
#define INTERFACEHANDLER_H

#include <QMainWindow>
#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QStackedWidget>
#include <QStyle>

class InterfaceHandler
{
public:
    InterfaceHandler(QMainWindow* parent);
    void setup();
    void setPlayback(QJsonObject playback);
    void disconnect();

private:
    QMainWindow* parent;

    QStackedWidget* widget;
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
