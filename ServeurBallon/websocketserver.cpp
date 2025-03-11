// WebSocketServer.cpp
#include "websocketserver.h"

WebSocketServer::WebSocketServer(quint16 port, QObject *parent)
    : QObject(parent)
{
    m_server = new QWebSocketServer("KISS Server", QWebSocketServer::NonSecureMode, this);
    if (m_server->listen(QHostAddress::Any, port)) {
        emit logMessage(QString("WebSocket server listening on port %1").arg(port));
        connect(m_server, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
    } else {
        emit logMessage("Erreur : impossible de lancer le serveur WebSocket !");
    }
}

WebSocketServer::~WebSocketServer() {
    m_server->close();
    qDeleteAll(m_clients);
}

void WebSocketServer::onNewConnection() {
    QWebSocket *client = m_server->nextPendingConnection();
    connect(client, &QWebSocket::textMessageReceived, this, &WebSocketServer::onTextMessageReceived);
    connect(client, &QWebSocket::disconnected, this, &WebSocketServer::onClientDisconnected);
    m_clients << client;
    emit logMessage("Nouveau client WebSocket connecté.");
}

void WebSocketServer::onTextMessageReceived(const QString &message) {
    emit logMessage("Message WS reçu: " + message);
}

void WebSocketServer::onClientDisconnected() {
    QWebSocket *client = qobject_cast<QWebSocket*>(sender());
    if(client) {
        m_clients.removeAll(client);
        client->deleteLater();
        emit logMessage("Client WebSocket déconnecté.");
    }
}

void WebSocketServer::broadcastMessage(const QString &msg) {
    for (QWebSocket *client : m_clients) {
        if(client->state() == QAbstractSocket::ConnectedState)
            client->sendTextMessage(msg);
    }
}
