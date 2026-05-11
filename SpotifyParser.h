#ifndef SPOTIFYPARSER_H
#define SPOTIFYPARSER_H

#include <QJsonObject>
#include <QJsonArray>

class SpotifyParser
{
public:
    SpotifyParser();
    static QJsonObject parsePlayback(QJsonObject playback);
};

#endif // SPOTIFYPARSER_H
