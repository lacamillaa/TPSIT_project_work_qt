#include "SpotifyElaborator.h"
#include <qjsonobject.h>

void SpotifyElaborator::setAccessToken(QString access_token) {
    this->access_token = access_token;
}

void SpotifyElaborator::returnImageColors(QString imageUrl) {
    QNetworkAccessManager manager;
    QNetworkRequest req(imageUrl);
    QNetworkReply *reply = manager.get(req);
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