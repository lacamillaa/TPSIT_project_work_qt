#include "InterfaceHandler.h"

#include <qnetworkreply.h>

InterfaceHandler::InterfaceHandler(QMainWindow* parent) {
    this->parent = parent;
}

void InterfaceHandler::setup() {
    QWidget *fullLayout = new QWidget();
    fullLayout->setMinimumSize(parent->width(), parent->height());

    // Il QStackedWidget ci permette di switchare tra "WIDGET 1" e "WIDGET 2"
    QStackedWidget *stackedWidget = new QStackedWidget();
    this->stackedWidget = stackedWidget;

    // ==========================================
    // WIDGET 1: LAYOUT DI RIPRODUZIONE
    // ==========================================
    QWidget *widget1 = new QWidget();
    this->widget1 = widget1;
    widget1->setStyleSheet("background-color: pink");
    QVBoxLayout *layoutWidget1 = new QVBoxLayout(widget1);

    // --- DISPLAY BOX ---
    QWidget *displayBox = new QWidget();
    this->displayBox = displayBox;
    displayBox->setStyleSheet("border: 2px solid black; border-radius: 5px;");
    displayBox->setFixedSize(400, 180); // Dimensione fissa per il display box
    QVBoxLayout *displayBoxLayout = new QVBoxLayout(displayBox);

    // Parte superiore: Copertina + Info
    QHBoxLayout *topInfoLayout = new QHBoxLayout();

    // Copertina
    QLabel *copertina = new QLabel();
    this->copertina = copertina;
    copertina->setFixedSize(120, 120);
    copertina->setStyleSheet("border: 1px solid gray; background-color: #e0e0e0");
    copertina->setAlignment(Qt::AlignCenter);

    // Info (Titolo, Artista, Album, Anno)
    QVBoxLayout *infoLayout = new QVBoxLayout();
    QLabel *titolo = new QLabel("<b>TITOLO</b>");
    this->titolo = titolo;
    titolo->setStyleSheet("border: none; font-size: 16px");
    QLabel *artistaAlbum = new QLabel("artista - album");
    this->artistaAlbum = artistaAlbum;
    artistaAlbum->setStyleSheet("border: none;");
    QLabel *annoUscita = new QLabel("anno di uscita");
    annoUscita->setStyleSheet("border: none;");
    this->annoUscita = annoUscita;

    infoLayout->addWidget(titolo);
    infoLayout->addWidget(artistaAlbum);
    infoLayout->addWidget(annoUscita);
    infoLayout->setAlignment(Qt::AlignVCenter);

    topInfoLayout->addWidget(copertina);
    topInfoLayout->addSpacing(15);
    topInfoLayout->addLayout(infoLayout);

    // Parte inferiore: Barra di avanzamento
    QHBoxLayout *progressLayout = new QHBoxLayout();
    QLabel *currentTime = new QLabel("0:07");
    this->currentTime = currentTime;
    currentTime->setStyleSheet("border: none;");

    QSlider *progressBar = new QSlider(Qt::Horizontal);
    progressBar->setStyleSheet("border: none;");
    progressBar->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    progressBar->setFocusPolicy(Qt::NoFocus);
    progressBar->setRange(0, 100);
    this->progressBar = progressBar;

    QLabel *totalTime = new QLabel("3:21");
    totalTime->setStyleSheet("border: none;");
    this->totalTime = totalTime;

    progressLayout->addWidget(currentTime);
    progressLayout->addWidget(progressBar);
    progressLayout->addWidget(totalTime);

    // Assemblo il Display Box
    displayBoxLayout->addLayout(topInfoLayout);
    displayBoxLayout->addSpacing(10);
    displayBoxLayout->addLayout(progressLayout);

    // Centro il Display Box all'interno del WIDGET 1
    layoutWidget1->addStretch();
    layoutWidget1->addWidget(displayBox, 0, Qt::AlignCenter);
    layoutWidget1->addStretch();

    // ==========================================
    // WIDGET 2: LAYOUT ERRORE / VUOTO
    // ==========================================
    QWidget *widget2 = new QWidget();
    widget2->setStyleSheet("background-color: rgb(30, 30, 30); color: white;");
    QVBoxLayout *layoutWidget2 = new QVBoxLayout(widget2);

    QLabel *messaggioErrore = new QLabel("Si è verificato un errore");
    this->messaggioErrore = messaggioErrore;
    messaggioErrore->setAlignment(Qt::AlignCenter);
    messaggioErrore->setStyleSheet("font-size: 18px; border: 1px solid black; padding: 50px;");

    // Centro il messaggio di errore all'interno del WIDGET 2
    layoutWidget2->addStretch();
    layoutWidget2->addWidget(messaggioErrore, 0, Qt::AlignCenter);
    layoutWidget2->addStretch();


    // ==========================================
    // ASSEMBLAGGIO FINALE
    // ==========================================
    stackedWidget->addWidget(widget1); // Indice 0
    stackedWidget->addWidget(widget2); // Indice 1

    // Layout principale della finestra che contiene lo stack
    QVBoxLayout *mainLayout = new QVBoxLayout();

    parent->setCentralWidget(stackedWidget);
}

void InterfaceHandler::setPlayback(QJsonObject playback) {
    if(playback.value("is_valid").toBool()) {
        this->stackedWidget->setCurrentIndex(0);
        QString titolo = playback.value("name").toString();
        QFontMetrics metrics(this->titolo->font());
        titolo = metrics.elidedText(titolo, Qt::ElideRight, this->titolo->width());
        this->titolo->setText(titolo);
        QString main_artist = playback.value("main_artist").toString();
        QString album = playback.value("album_name").toString();
        QString format = QString("%1 - %2").arg(main_artist, album);
        QFontMetrics metrics2(this->titolo->font());
        format = metrics2.elidedText(format, Qt::ElideRight, this->artistaAlbum->width());
        this->artistaAlbum->setText(format);
        this->annoUscita->setText(playback.value("release_date").toString());
        int progress = playback.value("offset").toInt();
        int duration = playback.value("duration").toInt();
        progress /= 1000;
        duration /= 1000;
        int minutes = progress / 60;
        int seconds = progress % 60;
        QString format_time = QString("%1%2:%3%4")
            .arg(minutes < 10 ? "0" : "")
            .arg(minutes)
            .arg(seconds < 10 ? "0" : "")
            .arg(seconds);
        this->currentTime->setText(format_time);
        minutes = duration / 60;
        seconds = duration % 60;
        format_time = QString("%1%2:%3%4")
            .arg(minutes < 10 ? "0" : "")
            .arg(minutes)
            .arg(seconds < 10 ? "0" : "")
            .arg(seconds);
        this->totalTime->setText(format_time);
        this->progressBar->setValue(progress * 100 / duration);
        QJsonObject color = playback.value("color").toObject();
        int r, g, b;
        r = color.value("r").toInt();
        g = color.value("g").toInt();
        b = color.value("b").toInt();
        double luminance = (0.299 * r) + (0.587 * g) + (0.114 * b);
        QString textColor;
        if (luminance > 128) {
            textColor = "#3d3d3d";
        } else {
            textColor = "#ffffff";
        }
        this->displayBox->setStyleSheet(QString("color: %1;").arg(textColor));
        QString stylesheet = QString("background-color: rgb(%1, %2, %3)").arg(r).arg(g).arg(b);
        this->widget1->setStyleSheet(stylesheet);
        QString image_url = playback.value("album_cover").toObject().value("url").toString();
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        QNetworkRequest req((QUrl(image_url)));
        QNetworkReply *reply = manager->get(req);
        QObject::connect(reply, &QNetworkReply::finished, [this, reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                // Leggi i dati binari scaricati
                QByteArray imageData = reply->readAll();
                QPixmap pixmap;

                // Carica l'immagine dai byte
                if (pixmap.loadFromData(imageData)) {
                    // Usa la tua QLabel (es. coverLabel)
                    // Applica il ridimensionamento mantenendo le proporzioni
                    this->copertina->setPixmap(pixmap.scaled(this->copertina->size(),
                                                              Qt::KeepAspectRatio,
                                                              Qt::SmoothTransformation));
                }
            } else {
                this->copertina->setStyleSheet("color: black");
                this->copertina->setText("Errore nel caricamento della copertina");
            }
        });
    }
}

void InterfaceHandler::setError(QString errorString) {
    QString date = QDateTime::currentDateTime().toString();
    QString errorStr = QString("%1 %2").arg(errorString).arg(date);
    this->messaggioErrore->setText(errorStr);
}

void InterfaceHandler::disconnect() {
    this->setError("L'utente è stato disconnesso");
    this->stackedWidget->setCurrentIndex(1);
}