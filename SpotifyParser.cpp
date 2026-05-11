#include "SpotifyParser.h"

SpotifyParser::SpotifyParser() {}

QJsonObject SpotifyParser::parsePlayback(QJsonObject playback) {
    QJsonObject parse = QJsonObject();
    if(playback.value("currently_playing_type").toString("") != "track") {
        parse.insert("is_valid", false);
        return parse;
    }
    parse.insert("is_valid", true);
    parse.insert("is_playing", playback.value("is_playing").toBool());
    parse.insert("offset", playback.value("progress_ms").toInt());
    QJsonObject item = playback.value("item").toObject();
    parse.insert("id", item.value("id").toString());
    parse.insert("name", item.value("name").toString());
    parse.insert("duration", item.value("duration_ms").toInt());
    QJsonArray arr = item.value("artists").toArray();
    QJsonArray artists_arr;
    QStringList artists;
    for(const auto &value : arr) {
        QString val = value.toObject().value("name").toString();
        artists_arr.append(val);
        artists.append(val);
    }
    parse.insert("artists_array", artists_arr);
    parse.insert("artists_names", artists.join(", "));
    parse.insert("main_artist", arr.at(0).toObject().value("name").toString());
    QJsonObject album = item.value("album").toObject();
    parse.insert("album_name", album.value("name").toString());
    parse.insert("album_cover", album.value("images")[0].toObject());
    parse.insert("release_date", album.value("release_date").toString().split("-")[0]);
    parse.insert("album_id", album.value("id").toString());
    return parse;
}
