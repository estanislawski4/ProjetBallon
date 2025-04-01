#include "serialportmanager.h"
#include <QDebug>
#include <QSerialPortInfo>

/**
 * @file SerialPortManager.cpp
 * @brief Implémentation de la classe SerialPortManager.
 *
 * Ce fichier contient l'implémentation des méthodes de la classe SerialPortManager qui gère
 * la communication via un port série en utilisant QSerialPort.
 */

/**
 * @brief Constructeur de la classe SerialPortManager.
 *
 * Initialise l'objet SerialPortManager et connecte les signaux de QSerialPort aux slots
 * onReadyRead et onErrorOccurred pour la gestion de la lecture des données et des erreurs.
 *
 * @param parent Pointeur vers l'objet parent (par défaut nullptr).
 */
SerialPortManager::SerialPortManager(QObject *parent)
    : QObject(parent)
{
    connect(&m_serial, &QSerialPort::readyRead, this, &SerialPortManager::onReadyRead);
    connect(&m_serial, &QSerialPort::errorOccurred, this, &SerialPortManager::onErrorOccurred);
}

/**
 * @brief Retourne la liste des ports série disponibles.
 *
 * Parcourt la liste des ports disponibles fournie par QSerialPortInfo et retourne les noms de port.
 *
 * @return QStringList Liste des noms de ports disponibles.
 */
QStringList SerialPortManager::availablePorts() const
{
    QStringList ports;
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts())
        ports << info.portName();
    return ports;
}

/**
 * @brief Ouvre le port série spécifié.
 *
 * Configure le port série avec des paramètres par défaut (9600 bauds, 8 bits, pas de parité,
 * 1 bit d'arrêt, pas de contrôle de flux) et tente de l'ouvrir en mode lecture-écriture.
 *
 * @param portName Nom du port à ouvrir.
 * @param errorString Référence à une chaîne pour retourner le message d'erreur en cas d'échec.
 * @return bool @c true si le port a été ouvert avec succès, @c false sinon.
 */
bool SerialPortManager::openPort(const QString &portName, QString &errorString)
{
    bool success = true;
    m_serial.setPortName(portName);
    m_serial.setBaudRate(QSerialPort::Baud9600);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serial.open(QIODevice::ReadWrite)) {
        errorString = m_serial.errorString();
        success = false;
    }
    return success;
}

/**
 * @brief Écrit des données sur le port série.
 *
 * Envoie le tableau d'octets fourni via le port série ouvert.
 *
 * @param data Les données à envoyer.
 * @return qint64 Le nombre d'octets écrits, ou une valeur négative en cas d'erreur.
 */
qint64 SerialPortManager::writeData(const QByteArray &data)
{
    return m_serial.write(data);
}

/**
 * @brief Ferme le port série.
 *
 * Ferme le port s'il est actuellement ouvert.
 */
void SerialPortManager::closePort()
{
    if (m_serial.isOpen())
        m_serial.close();
}

/**
 * @brief Slot appelé lorsque des données sont prêtes à être lues.
 *
 * Lit toutes les données disponibles sur le port série et émet le signal @c dataReceived.
 */
void SerialPortManager::onReadyRead()
{
    QByteArray data = m_serial.readAll();
    emit dataReceived(data);
}

/**
 * @brief Slot appelé en cas d'erreur sur le port série.
 *
 * Vérifie l'erreur retournée par QSerialPort et, si celle-ci n'est pas QSerialPort::NoError,
 * émet le signal @c errorOccurred avec la description de l'erreur.
 *
 * @param error Code d'erreur retourné par QSerialPort.
 */
void SerialPortManager::onErrorOccurred(QSerialPort::SerialPortError error)
{
    if(error != QSerialPort::NoError)
        emit errorOccurred(m_serial.errorString());
}
