#ifndef SPOTIFYELABORATOR_H
#define SPOTIFYELABORATOR_H

#include "SpotifyParser.h"

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

class SpotifyElaborator : public QObject
{
    Q_OBJECT
public:
    explicit SpotifyElaborator(QObject *parent);
    void returnImageColor(QString imageUrl);
    void setAccessToken(QString access_token);
    void makePlaybackRequest();

private:
    QNetworkAccessManager *manager;
    QTimer *timer;
    QTimer *interval_timer;

    QString access_token;
    int local_progress;
    bool local_is_playing;
    QString currently_playing;

    void resetTimer(int newTimeout);
};

#endif // SPOTIFYELABORATOR_H
