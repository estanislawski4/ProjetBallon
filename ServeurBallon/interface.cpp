#include "interface.h"
#include "ui_interface.h"

#include "serialportmanager.h"
#include "aprsisclient.h"
#include "ax25converter.h"
#include "kisshandler.h"
#include "mysqlmanager.h"

#include <QDebug>

/**
 * @file interface.cpp
 * @brief Implémentation de la classe Interface.
 *
 * Ce fichier contient l'implémentation de la classe Interface qui gère l'interface utilisateur,
 * la communication via les ports série, l'envoi des trames vers APRS-IS et LoRa, et leur stockage en base de données.
 */

/**
 * @brief Constructeur de la classe Interface.
 *
 * Initialise l'interface utilisateur et instancie les différents gestionnaires
 * (SerialPortManager, APRSISClient, AX25Converter, KISSHandler, MySQLManager). Configure les connexions
 * entre les signaux et les slots pour rediriger les messages vers le log.
 *
 * @param parent Pointeur vers l'objet parent (par défaut nullptr).
 */
Interface::Interface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Interface)
{
    ui->setupUi(this);

    // Instanciation des gestionnaires
    m_serialManager = new SerialPortManager(this);
    m_aprsClient    = new APRSISClient(this);
    m_converter     = new AX25Converter(this);
    m_kissHandler   = new KISSHandler(m_aprsClient, m_converter, this);

    // Connexion à la base de données
    m_dbManager = new MySQLManager(this);
    if (!m_dbManager->openConnection()) {
        ui->logs->append("Erreur de connexion à la base MySQL !");
    } else {
        ui->logs->append("Connexion MySQL établie.");
    }

    // Redirection de la journalisation vers l'interface
    connect(m_serialManager, &SerialPortManager::errorOccurred, this, [this](const QString &err) {
        ui->logs->append("Erreur série: " + err);
    });
    connect(m_kissHandler, &KISSHandler::logMessage, this, [this](const QString &msg) {
        ui->logs->append(msg);
    });
    connect(m_aprsClient, &APRSISClient::messageReceived, this, [this](const QString &msg) {
        ui->logs->append("APRS-IS >> " + msg);
    });

    // Traitement et stockage des trames LoRa reçues
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

    // Initialisation des ports série et connexion au serveur APRS-IS
    fillPortsComboBox();
    m_aprsClient->connectToServer("france.aprs2.net", 14580);
}

/**
 * @brief Destructeur de la classe Interface.
 *
 * Ferme la connexion à la base de données et libère la mémoire allouée pour l'interface utilisateur.
 */
Interface::~Interface()
{
    if (m_dbManager)
        m_dbManager->closeConnection();
    delete ui;
}

/**
 * @brief Remplit la combobox avec les ports série disponibles.
 *
 * Récupère la liste des ports via SerialPortManager et met à jour l'interface en conséquence.
 * Affiche un message dans le log si aucun port n'est détecté.
 */
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

/**
 * @brief Gère l'action du bouton "Start".
 *
 * Tente d'ouvrir le port série sélectionné et affiche le résultat (succès ou erreur) dans le log.
 */
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

/**
 * @brief Construit une trame APRS à partir des informations saisies.
 *
 * Récupère le source, la destination et le message saisis dans l'interface pour
 * construire une trame APRS respectant le format requis.
 *
 * @return QString La trame APRS construite.
 */
QString Interface::buildAprsFrame()
{
    QString source = ui->sourceLineEdit->text().trimmed();
    if (source.isEmpty())
        source = "F4LTZ";

    QString aprsHeader = "APRS,TCPIP*,qAC," + source;
    QString dest = ui->destLineEdit->text().trimmed();
    if (dest.isEmpty())
        dest = "F4GOH";
    dest = dest.leftJustified(9, ' ');
    QString payload = ui->messageLineEdit->text().trimmed();

    return QString("%1>%2::%3:%4")
        .arg(source)
        .arg(aprsHeader)
        .arg(dest)
        .arg(payload);
}

/**
 * @brief Construit une trame LoRa à partir des informations saisies.
 *
 * Récupère le source, la destination et le message depuis l'interface pour
 * construire une trame LoRa au format TNC2.
 *
 * @return QString La trame LoRa construite.
 */
QString Interface::buildLoRaFrame()
{
    QString source = ui->sourceLineEdit->text().trimmed();
    if (source.isEmpty())
        source = "F4LTZ";

    QString loraHeader = "APIN21,WIDE1-1";
    QString dest = ui->destLineEdit->text().trimmed();
    if (dest.isEmpty())
        dest = "F4KMN";
    dest = dest.leftJustified(9, ' ');
    QString payload = ui->messageLineEdit->text().trimmed();

    return QString("%1>%2::%3:%4")
        .arg(source)
        .arg(loraHeader)
        .arg(dest)
        .arg(payload);
}

/**
 * @brief Stocke une trame LoRa dans la base de données.
 *
 * Vérifie si la machine source et la machine destination existent dans la base de données,
 * les insère automatiquement si nécessaire, puis stocke la trame et son message associé.
 *
 * @param source Indicatif de la machine source.
 * @param destination Indicatif de la machine destination.
 * @param fullTrame La trame LoRa complète à stocker.
 * @param message Le message extrait de la trame.
 * @return bool @c true si le stockage a réussi, @c false sinon.
 */
bool Interface::storeLoRaTrame(const QString &source, const QString &destination,
                               const QString &fullTrame, const QString &message)
{
    bool success = true;

    // Vérifier et insérer la machine source si nécessaire
    if (!m_dbManager->machineExists(source)) {
        if (!m_dbManager->insertMachine(source, "Machine ajoutée automatiquement")) {
            ui->logs->append("Erreur lors de l'insertion de la machine source dans la BDD.");
            success = false;
        }
    }

    // Vérifier et insérer la machine destination si nécessaire (uniquement si la précédente a réussi)
    if (success && !m_dbManager->machineExists(destination)) {
        if (!m_dbManager->insertMachine(destination, "Machine ajoutée automatiquement")) {
            ui->logs->append("Erreur lors de l'insertion de la machine destination dans la BDD.");
            success = false;
        }
    }

    // Si tout est en ordre, procéder à l'insertion de la trame
    if (success) {
        QString queryStr = QString("INSERT INTO trames (source, destination, trame, message) "
                                   "VALUES ('%1', '%2', '%3', '%4')")
                               .arg(source)
                               .arg(destination)
                               .arg(fullTrame)
                               .arg(message);
        if (!m_dbManager->executeNonQuery(queryStr)) {
            ui->logs->append("Erreur DB: " + m_dbManager->database().lastError().text());
            success = false;
        }
    }

    return success;
}

/**
 * @brief Gère l'envoi d'une trame.
 *
 * Vérifie que le message à envoyer n'est pas vide, construit et envoie une trame APRS et une trame LoRa.
 * La trame LoRa est convertie en trame AX.25 avant d'être transmise via le port série, puis stockée en base.
 */
void Interface::onSendButtonClicked()
{
    QString message = ui->messageLineEdit->text().trimmed();
    if (message.isEmpty()) {
        ui->logs->append("Aucun message à envoyer !");
        return;
    }

    // Envoyer la trame APRS
    QString aprsFrame = buildAprsFrame();
    m_aprsClient->sendLine(aprsFrame + "\r\n");
    ui->logs->append("Message APRS envoyé : " + aprsFrame);

    // Construire et envoyer la trame LoRa
    QString loraTNC2 = buildLoRaFrame();
    qDebug() << "Trame LoRa TNC2 :" << loraTNC2;

    QByteArray ax25Frame = m_converter->convertTNC2ToAX25(loraTNC2);
    if (ax25Frame.isEmpty()) {
        ui->logs->append("Erreur conversion AX.25 (LoRa) !");
        return;
    }

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

    qint64 written = m_serialManager->writeData(kissFrame);
    if (written < 0) {
        ui->logs->append("Erreur d'envoi sur le port série (LoRa) !");
    } else {
        ui->logs->append("Trame LoRa envoyée (hex) : " + QString(kissFrame.toHex(' ')));
    }

    QString sourceStr = ui->sourceLineEdit->text().trimmed();
    if (sourceStr.isEmpty())
        sourceStr = "F4LTZ";
    QString destStr = ui->destLineEdit->text().trimmed();
    if (destStr.isEmpty())
        destStr = "F4KMN";
    int pos = loraTNC2.lastIndexOf(':');
    QString messageUtil = (pos != -1) ? loraTNC2.mid(pos + 1).trimmed() : "";

    if (storeLoRaTrame(sourceStr, destStr, loraTNC2, messageUtil)) {
        ui->logs->append("Trame LoRa stockée dans la BDD.");
    } else {
        ui->logs->append("Erreur lors du stockage de la trame dans la BDD.");
    }
}
