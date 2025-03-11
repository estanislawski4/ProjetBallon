#include "interface.h"
#include "ui_interface.h"

#include "serialportmanager.h"
#include "aprsisclient.h"
#include "ax25converter.h"
#include "kisshandler.h"
#include "mysqlmanager.h"

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

    // Instanciation et connexion à la base de données
    m_dbManager = new MySQLManager(this);
    if (!m_dbManager->openConnection()) {
        ui->logs->append("Erreur de connexion à la base MySQL !");
    } else {
        ui->logs->append("Connexion MySQL établie.");
    }

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

    // Dans le constructeur de Interface, après l'initialisation de m_kissHandler :
    connect(m_kissHandler, &KISSHandler::loRaFrameReceived, this,
            [this](const QString &src, const QString &dest, const QString &fullTrame, const QString &msg) {
                if (storeLoRaTrame(src, dest, fullTrame, msg)) {
                    ui->logs->append("Trame LoRa reçue stockée dans la BDD.");
                } else {
                    ui->logs->append("Erreur lors du stockage de la trame reçue dans la BDD.");
                }
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
    connect(ui->aprsCheckBox, &QCheckBox::toggled,
            m_kissHandler, &KISSHandler::setSendToAprs);

    m_kissHandler->setSendToAprs(ui->aprsCheckBox->isChecked());

    // Remplir la liste des ports et initier la connexion APRS-IS
    fillPortsComboBox();
    m_aprsClient->connectToServer("france.aprs2.net", 14580);
}

Interface::~Interface()
{
    // Fermer la connexion à la BDD si nécessaire
    if (m_dbManager) {
        m_dbManager->closeConnection();
    }
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

QString Interface::buildAprsFrame() {
    // On suppose que vous disposez de QLineEdit dédiés pour ces paramètres
    QString source = ui->sourceLineEdit->text().trimmed();
    if (source.isEmpty())
        source = "F4LTZ";  // valeur par défaut

    // L'en-tête APRS est fixe dans cet exemple.
    QString aprsHeader = "APRS,TCPIP*,qAC," + source;

    QString dest = ui->destLineEdit->text().trimmed();
    if (dest.isEmpty())
        dest = "F4GOH";
    // On complète à 9 caractères si nécessaire
    dest = dest.leftJustified(9, ' ');

    QString payload = ui->messageLineEdit->text().trimmed();

    // Format final : SOURCE > en-tête :: DEST : payload
    return QString("%1>%2::%3:%4")
        .arg(source)
        .arg(aprsHeader)
        .arg(dest)
        .arg(payload);
}

// Fonction pour construire la trame LoRa (qui sera convertie)
QString Interface::buildLoRaFrame() {
    // Pour LoRa, on peut utiliser des QLineEdit différents
    QString source = ui->sourceLineEdit->text().trimmed();
    if (source.isEmpty())
        source = "F4LTZ";  // valeur par défaut

    // En-tête spécifique LoRa
    QString loraHeader = "APIN21,WIDE1-1";

    QString dest = ui->destLineEdit->text().trimmed();
    if (dest.isEmpty())
        dest = "F4KMN";
    dest = dest.leftJustified(9, ' ');

    QString payload = ui->messageLineEdit->text().trimmed();

    // Format final : SOURCE > loraHeader :: DEST : payload
    return QString("%1>%2::%3:%4")
        .arg(source)
        .arg(loraHeader)
        .arg(dest)
        .arg(payload);

}

bool Interface::storeLoRaTrame(const QString &source, const QString &destination,
                               const QString &fullTrame, const QString &message)
{
    // Vérifier si la machine source existe, sinon l'insérer.
    if (!m_dbManager->machineExists(source)) {
        if (!m_dbManager->insertMachine(source, "Machine ajoutée automatiquement")) {
            ui->logs->append("Erreur lors de l'insertion de la machine source dans la BDD.");
            return false;
        }
    }
    // Vérifier si la machine destination existe, sinon l'insérer.
    if (!m_dbManager->machineExists(destination)) {
        if (!m_dbManager->insertMachine(destination, "Machine ajoutée automatiquement")) {
            ui->logs->append("Erreur lors de l'insertion de la machine destination dans la BDD.");
            return false;
        }
    }

    QString queryStr = QString("INSERT INTO trames (source, destination, trame, message) "
                               "VALUES ('%1', '%2', '%3', '%4')")
                           .arg(source)
                           .arg(destination)
                           .arg(fullTrame)
                           .arg(message);
    if (!m_dbManager->executeNonQuery(queryStr)) {
        ui->logs->append("Erreur DB: " + m_dbManager->database().lastError().text());
        return false;
    }
    return true;
}

void Interface::onSendButtonClicked()
{
    // Vérifier que l'utilisateur a saisi un message
    QString message = ui->messageLineEdit->text().trimmed();
    if (message.isEmpty()) {
        ui->logs->append("Aucun message à envoyer !");
        return;
    }

    // Créer la trame APRS et l'envoyer directement sur aprs.fi
    QString aprsFrame = buildAprsFrame();
    m_aprsClient->sendLine(aprsFrame + "\r\n");
    ui->logs->append("Message APRS envoyé : " + aprsFrame);

    // Créer la trame LoRa qui doit être convertie
    QString loraTNC2 = buildLoRaFrame();
    qDebug() << "Trame LoRa TNC2 :" << loraTNC2;

    // Convertir la trame LoRa (TNC2) en AX.25
    QByteArray ax25Frame = m_converter->convertTNC2ToAX25(loraTNC2);
    if (ax25Frame.isEmpty()) {
        ui->logs->append("Erreur conversion AX.25 (LoRa) !");
        return;
    }

    // Construire la trame KISS pour le LoRa
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

    // Envoi de la trame KISS sur le port série (LoRa)
    qint64 written = m_serialManager->writeData(kissFrame);
    if (written < 0) {
        ui->logs->append("Erreur d'envoi sur le port série (LoRa) !");
    } else {
        ui->logs->append("Trame LoRa envoyée (hex) : " + QString(kissFrame.toHex(' ')));
    }

    // Stocker la trame LoRa dans la base de données
    QString sourceStr = ui->sourceLineEdit->text().trimmed();
    if (sourceStr.isEmpty())
        sourceStr = "F4LTZ";
    QString destStr = ui->destLineEdit->text().trimmed();
    if (destStr.isEmpty())
        destStr = "F4KMN";
    // Extraire le message utile : tout ce qui suit le dernier ':'
    int pos = loraTNC2.lastIndexOf(':');
    QString messageUtil = (pos != -1) ? loraTNC2.mid(pos + 1).trimmed() : "";

    if (storeLoRaTrame(sourceStr, destStr, loraTNC2, messageUtil)) {
        ui->logs->append("Trame LoRa stockée dans la BDD.");
    } else {
        ui->logs->append("Erreur lors du stockage de la trame dans la BDD.");
    }
}
