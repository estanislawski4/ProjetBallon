#include "aprsisclient.h"
#include <QDebug>

/**
 * @brief Constructeur de la classe APRSISClient.
 *
 * Initialise le client APRS-IS avec les informations de connexion (call sign et mot de passe)
 * et connecte les signaux du socket aux slots correspondants.
 *
 * @param parent Pointeur vers l'objet parent (par défaut nullptr).
 */
APRSISClient::APRSISClient(QObject *parent)
    : QObject(parent),
    mIGateCall("F4LTZ"),
    mIGatePass("9090")
{
    // Connecter les signaux du socket aux slots appropriés
    connect(&m_socket, &QTcpSocket::connected, this, &APRSISClient::onConnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, &APRSISClient::onReadyRead);
    connect(&m_socket, &QTcpSocket::disconnected, this, &APRSISClient::onDisconnected);
}

/**
 * @brief Établit une connexion avec le serveur APRS-IS.
 *
 * Tente d'ouvrir une connexion TCP vers le serveur spécifié par l'adresse @p host et le port @p port.
 *
 * @param host Adresse du serveur APRS-IS.
 * @param port Port de connexion.
 */
void APRSISClient::connectToServer(const QString &host, int port)
{
    m_socket.connectToHost(host, port);
}

/**
 * @brief Slot appelé lorsque la connexion au serveur est établie.
 *
 * Une fois connecté, ce slot émet le signal @c connected et envoie la ligne de login
 * pour authentifier l'utilisateur auprès du serveur APRS-IS.
 */
void APRSISClient::onConnected()
{
    emit connected();
    // Compose la ligne de login APRS-IS
    QString loginLine = QString("user %1 pass %2 vers QtIGATE 0.1\n")
                            .arg(mIGateCall)
                            .arg(mIGatePass);
    sendLine(loginLine);
}

/**
 * @brief Slot appelé lorsque des données sont disponibles sur le socket.
 *
 * Lit toutes les données disponibles et émet le signal @c messageReceived avec le contenu reçu.
 */
void APRSISClient::onReadyRead()
{
    QByteArray data = m_socket.readAll();
    emit messageReceived(QString::fromLatin1(data));
}

/**
 * @brief Slot appelé lors de la déconnexion du serveur APRS-IS.
 *
 * Émet le signal @c disconnected pour notifier que la connexion a été interrompue.
 */
void APRSISClient::onDisconnected()
{
    emit disconnected();
}

/**
 * @brief Envoie une ligne de trame vers le serveur APRS-IS.
 *
 * Avant l'envoi, vérifie que le socket est bien connecté. Si ce n'est pas le cas,
 * émet le signal @c errorOccurred avec un message d'erreur.
 *
 * @param line La ligne de trame à envoyer.
 */
void APRSISClient::sendLine(const QString &line)
{
    if(m_socket.state() == QAbstractSocket::ConnectedState) {
        m_socket.write(line.toLatin1());
    } else {
        emit errorOccurred("APRS-IS socket non connecté.");
    }
}
