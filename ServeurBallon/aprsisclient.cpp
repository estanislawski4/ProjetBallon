// APRSISClient.cpp
#include "aprsisclient.h"
#include <QDebug>

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

void APRSISClient::connectToServer(const QString &host, int port)
{
    m_socket.connectToHost(host, port);
}

void APRSISClient::onConnected()
{
    emit connected();
    // Compose la ligne de login APRS-IS
    QString loginLine = QString("user %1 pass %2 vers QtIGATE 0.1\n")
                            .arg(mIGateCall)
                            .arg(mIGatePass);
    sendLine(loginLine);
}

void APRSISClient::onReadyRead()
{
    QByteArray data = m_socket.readAll();
    emit messageReceived(QString::fromLatin1(data));
}

void APRSISClient::onDisconnected()
{
    emit disconnected();
}

void APRSISClient::sendLine(const QString &line)
{
    if(m_socket.state() == QAbstractSocket::ConnectedState) {
        m_socket.write(line.toLatin1());
    } else {
        emit errorOccurred("APRS-IS socket non connecté.");
    }
}
