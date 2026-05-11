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

class SpotifyElaborator : public QObject
{
    Q_OBJECT
public:
    static void returnImageColors(QString imageUrl);
    void setAccessToken(QString access_token);

private:
    QString access_token;
};

#endif // SPOTIFYELABORATOR_H
