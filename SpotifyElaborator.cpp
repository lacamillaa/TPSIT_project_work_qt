#include "SpotifyElaborator.h"
#include <qjsonobject.h>

SpotifyElaborator::SpotifyElaborator(QObject *parent) {
    this->local_is_playing = false;
    this->manager = new QNetworkAccessManager();
    this->timer = new QTimer();
    this->interval_timer = new QTimer();
    const int interval = 200;
    connect(interval_timer, &QTimer::timeout, this, [this](){
        if(this->local_is_playing) {
            this->local_progress += interval;
        }
    });
    this->interval_timer->start(interval);
}

void SpotifyElaborator::resetTimer(int newTimeout) {
    this->timer->stop();
    timer->singleShot(newTimeout, this, [this](){
        this->interval_timer->stop();
        qDebug() << "fine traccia!";
    });
}

void SpotifyElaborator::setAccessToken(QString access_token) {
    this->access_token = access_token;
}

void SpotifyElaborator::makePlaybackRequest() {
    QNetworkRequest request(QUrl("https://api.spotify.com/v1/me/player"));
    QString tok = "Bearer " + this->access_token;
    request.setRawHeader("Authorization", tok.toUtf8());
    request.setRawHeader("Content-Type", "application/json");
    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, [reply, this](){
        if(reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(response);
            QJsonObject obj = doc.object();
            QJsonObject result = SpotifyParser::parsePlayback(obj);
            QString id = result.value("id").toString();
            bool is_playing = result.value("is_playing").toBool();
            int duration = result.value("duration").toInt();
            int offset = result.value("offset").toInt();
            if(is_playing != this->local_is_playing) {
                if(!is_playing) {
                    qDebug() << "pausa";
                    this->timer->stop();
                }
                else {
                    qDebug() << "play";
                    this->resetTimer(duration - offset);
                }
            }
            this->local_is_playing = is_playing;
            if(this->currently_playing == id) {
                int diff = abs(this->local_progress - offset);
                if(diff > 1200) {
                    this->local_progress = offset;
                    this->resetTimer(duration - offset);
                }
                else {
                    this->local_progress = fmax(this->local_progress, offset);
                }
                local_progress = fmin(local_progress, duration);
            }
            else {
                this->resetTimer(duration - offset);
                this->currently_playing = id;
                QString track_name = result.value("name").toString();
                QString main_artist = result.value("main_artist").toString();
                qDebug() << "Now playing: " + track_name + " by " + main_artist;
                this->local_progress = result.value("offset").toInt();
                this->returnImageColor(
                    result.value("album_cover").toObject().value("url").toString()
                );
                this->interval_timer->start(1000);
            }
            int seconds = this->local_progress / 1000;
            qDebug() << "Progress:" << seconds / 60 << ":" << seconds % 60;
        }
    });
}

void SpotifyElaborator::returnImageColor(QString imageUrl) {
    QNetworkRequest req(imageUrl);
    QNetworkReply *reply = manager->get(req);
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QColor resultColor(Qt::black);
    if(reply->error() == QNetworkReply::NoError) {
        QByteArray arr = reply->readAll();
        QImage cover;
        cover.loadFromData(arr);
        if(cover.isNull()) {
            return;
        }
        QMap<QRgb, int> colorCount;
        const int offset = 10;
        for (int y = offset; y < cover.height() - offset; ++y) {
            for (int x = offset; x < cover.width() - offset; ++x) {
                QRgb pixel = cover.pixel(x, y);
                QColor c(pixel);

                if (c.value() < 30 || c.value() > 230 || c.saturation() < 30) {
                    continue;
                }

                const int scale_factor = 3;
                int r = (c.red() >> scale_factor) << scale_factor;
                int g = (c.green() >> scale_factor) << scale_factor;
                int b = (c.blue() >> scale_factor) << scale_factor;
                colorCount[qRgb(r, g, b)]++;
            }
        }
        QRgb dom_color;
        int max_value = 0;
        for (auto it = colorCount.begin(); it != colorCount.end(); it++) {
            if(it.value() > max_value) {
                max_value = it.value();
                dom_color = it.key();
            }
        }
        resultColor = QColor(dom_color);
        int r, g, b;
        resultColor.getRgb(&r, &g, &b);
        qDebug() << r << g << b;
    }
}