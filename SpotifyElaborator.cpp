#include "SpotifyElaborator.h"
#include <qjsonobject.h>

SpotifyElaborator::SpotifyElaborator(InterfaceHandler *interface) {
    this->local_is_playing = false;
    this->interface = interface;
    this->manager = new QNetworkAccessManager();
    this->timer = new QTimer();
    this->interval_timer = new QTimer();
    connect(interval_timer, &QTimer::timeout, this, [this](){
        if(this->local_is_playing) {
            this->local_progress += this->interval;
        }
    });
    this->interval_timer->start(this->interval);
}

void SpotifyElaborator::resetTimer(int newTimeout) {
    this->timer->stop();
    this->timer->singleShot(newTimeout, this, [this](){
        this->interval_timer->stop();
    });
}

void SpotifyElaborator::setAccessToken(QString access_token) {
    this->access_token = access_token;
}

void SpotifyElaborator::setLastFMKey(QString api_key) {
    this->lastfm_key = api_key;
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
                this->local_progress = fmin(this->local_progress, duration);
                result["offset"] = this->local_progress;
            }
            else {
                this->timer->stop();
                this->currently_playing = id;
                QString track_name = result.value("name").toString();
                QString main_artist = result.value("main_artist").toString();
                QString artist_names = result.value("artists_names").toString();
                qDebug() << "Now playing: " + track_name + " by " + main_artist;
                this->local_progress = result.value("offset").toInt();
                analyzeAudio(track_name, artist_names);
                this->local_color = this->returnImageColor(
                    result.value("album_cover").toObject().value("url").toString());
                this->interval_timer->start(this->interval);
            }
            QJsonObject color_rgb;
            int r, g, b;
            this->local_color.getRgb(&r, &g, &b);
            color_rgb["r"] = r;
            color_rgb["g"] = g;
            color_rgb["b"] = b;
            result["color"] = color_rgb;
            this->interface->setPlayback(result);
        }
    });
}

double SpotifyElaborator::analyzeAudio(QString track, QString artist_names) {
    return 0.5;
}

QColor SpotifyElaborator::returnImageColor(QString imageUrl) {
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
            return QColor(Qt::white);
        }
        QMap<QRgb, double> colorCount;
        // assegna valori in base alla dist. dal centro
        // dist. massima: lato * radq(2) / 2
        const double diag = cover.width() * sqrt(2) / 2;
        const int centerX = cover.width() / 2;
        const int centerY = cover.height() / 2;
        const int offset = 0;
        for (int y = offset; y < cover.height() - offset; ++y) {
            for (int x = offset; x < cover.width() - offset; ++x) {
                QRgb pixel = cover.pixel(x, y);
                QColor c(pixel);

                if (c.value() < 40 || c.value() > 225 || c.saturation() < 45) {
                    continue;
                }

                const int scale_factor = 2;
                int r = (c.red() >> scale_factor) << scale_factor;
                int g = (c.green() >> scale_factor) << scale_factor;
                int b = (c.blue() >> scale_factor) << scale_factor;
                double dist = pow(x - centerX, 2) + pow(y - centerY, 2);
                dist = sqrt(dist);
                // dà più peso ai pixel sul bordo e a quelli più luminosi
                colorCount[qRgb(r, g, b)] += (0.25 + dist / diag) * (c.value());
            }
        }
        QRgb dom_color(Qt::black);
        double max_value = 0;
        for (auto it = colorCount.begin(); it != colorCount.end(); it++) {
            if(it.value() > max_value) {
                max_value = it.value();
                dom_color = it.key();
            }
        }
        if(colorCount.isEmpty()) {
            int totalPixels = 0;
            int tot_r = 0, tot_g = 0, tot_b = 0;
            // calcola colore medio
            for (int y = offset; y < cover.height() - offset; ++y) {
                for (int x = offset; x < cover.width() - offset; ++x) {
                    QRgb pixel = cover.pixel(x, y);
                    QColor c(pixel);
                    int r, g, b;
                    c.getRgb(&r, &g, &b);
                    tot_r += r;
                    tot_g += g;
                    tot_b += b;
                    totalPixels++;
                }
            }
            dom_color = qRgb(tot_r / totalPixels, tot_g / totalPixels, tot_b / totalPixels);
        }
        resultColor = QColor(dom_color);
        return resultColor;
    }
    return resultColor;
}