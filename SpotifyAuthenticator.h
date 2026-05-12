#include "SpotifyParser.h"
#include "SpotifyElaborator.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

class SpotifyAuthenticator : public QObject {
    Q_OBJECT
public:
    explicit SpotifyAuthenticator(QString client_id, QString client_secret,
        QString redirect_uri);
    void startListening();
    void makeAuthCall();
    void disconnectUser();
    bool isConnected();

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    SpotifyElaborator *elab;
    QTcpServer *m_server;
    bool is_connected = false;
    QString client_id;
    QString client_secret;
    QString redirect_uri;
    QString access_token;
    QString refresh_token;
    QString scope;
    QString query_state;
    QString query_code;
    QDateTime expires;
    QTimer timer = QTimer();
    QNetworkAccessManager manager = QNetworkAccessManager();

    void exchangeCodeForToken(const QString *queryCode);
    void connectToPlayback(); // gestione dell'intervallo
    void refreshToken();
};
