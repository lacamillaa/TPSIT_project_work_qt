#include <QTcpServer>
#include <QTcpSocket>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>

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
    QTcpServer *m_server;
    bool is_connected = false;
    QString client_id;
    QString client_secret;
    QString redirect_uri;
    QString access_token;
    QString refresh_token;
    QString query_state;
    QString query_code;
    QDateTime expires;
    void exchangeCodeForToken(const QString *queryCode);
};
