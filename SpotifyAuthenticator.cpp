#include "spotifyauthenticator.h"

QString generateRandomString(int length) {
    const QString possible = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    QString str = "";
    for(int i = 0; i < length; i++) {
        float res = abs(rand() / (float)RAND_MAX);
        res *= possible.length();
        res = floor(res);
        str += possible[res];
    }
    return str;
}

SpotifyAuthenticator::SpotifyAuthenticator(QString client_id, QString client_secret,
    QString redirect_uri, QString lastfm, QMainWindow *window) {
    this->client_id = client_id;
    this->client_secret = client_secret;
    this->redirect_uri = redirect_uri;
    this->lastfm_api = lastfm;
    this->interface = new InterfaceHandler(window);
    this->interface->setup();
    m_server = new QTcpServer(this);
    elab = new SpotifyElaborator(this->interface);
    elab->setLastFMKey(lastfm);
    connect(m_server, &QTcpServer::newConnection, this, &SpotifyAuthenticator::onNewConnection);
}

void SpotifyAuthenticator::startListening() {
    if (!m_server->listen(QHostAddress::LocalHost, 3000)) {
        qCritical() << "Impossibile avviare il server sulla porta" << 3000;
    } else {
        qDebug() << "In attesa della callback di Spotify sulla porta" << 3000 << "...";
    }
}

void SpotifyAuthenticator::makeAuthCall() {
    QString scope = "user-read-private user-read-email user-read-playback-state";
    this->query_state = generateRandomString(64);
    QUrl authUrl = QUrl("https://accounts.spotify.com/authorize");
    QUrlQuery queryParams;
    queryParams.addQueryItem("response_type", "code");
    queryParams.addQueryItem("scope", scope);
    queryParams.addQueryItem("state", this->query_state);
    queryParams.addQueryItem("client_id", client_id);
    queryParams.addQueryItem("redirect_uri", redirect_uri);
    authUrl.setQuery(queryParams);
    QDesktopServices::openUrl(authUrl);
}

void SpotifyAuthenticator::onNewConnection() {
    QTcpSocket *socket = m_server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &SpotifyAuthenticator::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void SpotifyAuthenticator::onReadyRead() {
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) return;
    QByteArray data = socket->readAll();
    QString request = QString::fromUtf8(data);
    if (request.contains("GET /callback")) {
        QString firstLine = request.split("\r\n")[0];
        QString pathWithQuery = firstLine.split(" ")[1];
        QUrl url("http://localhost" + pathWithQuery);
        QUrlQuery query(url.query());
        QString code = query.queryItemValue("code");
        QString returnedState = query.queryItemValue("state");
        if (returnedState != this->query_state) {
            qWarning() << "ATTENZIONE: State non corrispondente! Possibile attacco CSRF.";
            socket->write("HTTP/1.1 401 Unauthorized\r\n\r\nStato non valido.");
        } else {
            this->query_code = code;
            exchangeCodeForToken(&this->query_code);
            QByteArray response = "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: text/html; charset=utf-8\r\n"
                                  "\r\n"
                                  "<html><body><h1>Autorizzazione eseguita con successo!</h1>"
                                  "<p>Puoi chiudere questa scheda e tornare all'app.</p></body></html>";
            socket->write(response);
            socket->flush();
        }
        socket->disconnectFromHost();
        m_server->close();
    }
}

void SpotifyAuthenticator::exchangeCodeForToken(const QString *queryCode) {
    QUrl url("https://accounts.spotify.com/api/token");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QString authString = client_id + ":" + client_secret;
    QByteArray authBase64 = authString.toUtf8().toBase64();
    request.setRawHeader("Authorization", "Basic " + authBase64);
    QUrlQuery params;
    params.addQueryItem("code", this->query_code);
    params.addQueryItem("redirect_uri", this->redirect_uri);
    params.addQueryItem("grant_type", "authorization_code");
    QNetworkReply *reply = manager.post(request, params.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply, &QNetworkReply::finished, [reply, this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObj = jsonDoc.object();
            this->access_token = jsonObj.value("access_token").toString();
            this->elab->setAccessToken(this->access_token);
            this->refresh_token = jsonObj.value("refresh_token").toString();
            this->scope = jsonObj.value("scope").toString();
            int exp = jsonObj.value("expires_in").toInt(3600);
            this->expires = QDateTime::currentDateTime().addSecs(exp);
            this->timer.singleShot((exp - 60) * 1000, this, &SpotifyAuthenticator::refreshToken);
            this->connectToPlayback();
        } else {
            qDebug() << "Errore richiesta Token:" << reply->errorString();
            qDebug() << "Status Code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << "Dettagli:" << reply->readAll();
        }
        reply->deleteLater();
    });
}

void SpotifyAuthenticator::refreshToken() {
    const long long secs_retry = 10;
    QDateTime moment = QDateTime::currentDateTime();
    if(this->expires.isValid() && moment >= this->expires.addSecs(-secs_retry)) {
        // mancano - di 10 secondi alla scadenza
        this->disconnectUser();
        return;
    }
    QUrl refresh_url("https://accounts.spotify.com/api/token");
    QUrlQuery query;
    query.addQueryItem("grant_type", "refresh_token");
    query.addQueryItem("refresh_token", this->refresh_token);
    QNetworkRequest req = QNetworkRequest(refresh_url);
    req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    QNetworkReply *reply = manager.post(req, query.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply, &QNetworkReply::finished, this, [this, reply, secs_retry](){
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObj = jsonDoc.object();
            this->access_token = jsonObj.value("access_token").toString();
            this->elab->setAccessToken(this->access_token);
            qDebug() << this->access_token;
            this->refresh_token = jsonObj.value("refresh_token").toString();
            this->scope = jsonObj.value("scope").toString();
            int exp = jsonObj.value("expires_in").toInt(3600);
            this->expires = QDateTime::currentDateTime().addSecs(exp);
            this->timer.singleShot((exp - 60) * 1000, this, &SpotifyAuthenticator::refreshToken);
            this->connectToPlayback();
        }
        else {
            qDebug() << "ERRORE nel refresh";
            // riprova dopo 15 secondi
            const int secs_retry = 10;
            this->timer.singleShot(secs_retry * 1000, this, &SpotifyAuthenticator::refreshToken);
        }
    });
}

void SpotifyAuthenticator::disconnectUser() {
    this->timer.stop();
    this->is_connected = false;
}

bool SpotifyAuthenticator::isConnected() {
    if (this->expires < QDateTime::currentDateTime()) {
        this->disconnectUser();
    }
    return this->is_connected;
}

void SpotifyAuthenticator::connectToPlayback() {
    this->is_connected = true;
    this->timer.start(1000);
    connect(&this->timer, &QTimer::timeout, elab, &SpotifyElaborator::makePlaybackRequest);
}