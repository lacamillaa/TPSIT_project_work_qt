#ifndef SPOTIFYELABORATOR_H
#define SPOTIFYELABORATOR_H

#include "SpotifyParser.h"
#include "InterfaceHandler.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QImage>
#include <QEventLoop>
#include <QColor>
#include <QTimer>
#include <QUrlQuery>

class SpotifyElaborator : public QObject
{
    Q_OBJECT
public:
    explicit SpotifyElaborator(InterfaceHandler *interface);
    QColor returnImageColor(QString imageUrl);
    void setAccessToken(QString access_token);
    void setLastFMKey(QString api_key);
    void makePlaybackRequest();
    double analyzeAudio(QString track, QString artist_names);

private:
    QNetworkAccessManager *manager;
    QTimer *timer;
    QTimer *interval_timer;
    InterfaceHandler *interface;

    const int interval = 200;

    QString access_token;
    QString lastfm_key;
    int local_progress;
    bool local_is_playing;
    QColor local_color;
    QString currently_playing;

    void resetTimer(int newTimeout);
};

#endif // SPOTIFYELABORATOR_H
