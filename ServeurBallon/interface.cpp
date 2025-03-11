#include "interface.h"
#include "ui_interface.h"

#include "serialportmanager.h"
#include "aprsisclient.h"
#include "ax25converter.h"
#include "kisshandler.h"
#include "websocketserver.h"

#include <QDebug>

Interface::Interface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Interface)
{
    ui->setupUi(this);

    // Instanciation des classes de gestion
    m_serialManager = new SerialPortManager(this);
    m_aprsClient    = new APRSISClient(this);
    m_converter     = new AX25Converter(this);
    m_kissHandler   = new KISSHandler(m_aprsClient, m_converter, this);
    m_wsServer      = new WebSocketServer(1234, this);

    // Connexions pour rediriger la journalisation dans l'interface
    connect(m_serialManager, &SerialPortManager::errorOccurred, this, [this](const QString &err) {
        ui->logs->append("Erreur série: " + err);
    });
    connect(m_kissHandler, &KISSHandler::logMessage, this, [this](const QString &msg) {
        ui->logs->append(msg);
    });
    connect(m_aprsClient, &APRSISClient::messageReceived, this, [this](const QString &msg) {
        ui->logs->append("APRS-IS >> " + msg);
    });
    connect(m_wsServer, &WebSocketServer::logMessage, this, [this](const QString &msg) {
        ui->logs->append("WebSocket: " + msg);
    });

    // Rediriger les données série vers le traitement KISS
    connect(m_serialManager, &SerialPortManager::dataReceived,
            m_kissHandler, &KISSHandler::parseKISSData);

    // Connexion des boutons de l'interface
    connect(ui->refreshButton, &QPushButton::clicked,
            this, &Interface::fillPortsComboBox);
    connect(ui->startButton, &QPushButton::clicked,
            this, &Interface::onStartButtonClicked);
    connect(ui->sendButton, &QPushButton::clicked,
            this, &Interface::onSendButtonClicked);

    // Remplir la liste des ports et initier la connexion APRS-IS
    fillPortsComboBox();
    m_aprsClient->connectToServer("france.aprs2.net", 14580);
}

Interface::~Interface()
{
    // Les objets enfants seront automatiquement supprimés
    delete ui;
}

void Interface::fillPortsComboBox()
{
    ui->portComboBox->clear();
    QStringList ports = m_serialManager->availablePorts();
    if (ports.isEmpty()) {
        ui->logs->append("Aucun port série détecté.");
    } else {
        ui->logs->append(QString("Détecté %1 port(s).").arg(ports.size()));
        for (const QString &port : ports)
            ui->portComboBox->addItem(port);
    }
}

void Interface::onStartButtonClicked()
{
    QString portName = ui->portComboBox->currentText();
    QString error;
    if (!m_serialManager->openPort(portName, error)) {
        ui->logs->append("Erreur : impossible d'ouvrir le port série ! Cause : " + error);
    } else {
        ui->logs->append("Port série ouvert : " + portName);
    }
}

void Interface::onSendButtonClicked()
{
    QString userText = ui->messageLineEdit->text().trimmed();
    if (userText.isEmpty()) {
        ui->logs->append("Aucun message à envoyer !");
        return;
    }

    // Conversion de TNC2 vers AX.25 via le convertisseur
    QByteArray ax25Frame = m_converter->convertTNC2ToAX25(userText);
    if (ax25Frame.isEmpty()) {
        ui->logs->append("Erreur conversion AX.25 !");
        return;
    }

    // Construction de la trame KISS
    QByteArray kissFrame;
    kissFrame.append((char)0xC0);
    kissFrame.append((char)0x00);
    for (unsigned char b : ax25Frame) {
        if (b == 0xC0) {
            kissFrame.append((char)0xDB);
            kissFrame.append((char)0xDC);
        } else if (b == 0xDB) {
            kissFrame.append((char)0xDB);
            kissFrame.append((char)0xDD);
        } else {
            kissFrame.append((char)b);
        }
    }
    kissFrame.append((char)0xC0);

    // Envoi de la trame sur le port série
    qint64 written = m_serialManager->writeData(kissFrame);
    if (written < 0) {
        ui->logs->append("Erreur d'envoi sur le port série !");
    } else {
        ui->logs->append("Trame envoyée (hex) : " + QString(kissFrame.toHex(' ')));
    }
}
