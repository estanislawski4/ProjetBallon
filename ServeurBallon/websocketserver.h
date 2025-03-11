#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

// WebSocketServer.h
#pragma once

#include <QObject>
#include <QList>
#include <QWebSocketServer>
#include <QWebSocket>

class WebSocketServer : public QObject {
    Q_OBJECT
public:
    explicit WebSocketServer(quint16 port, QObject *parent = nullptr);
    ~WebSocketServer();
    void broadcastMessage(const QString &msg);

signals:
    void logMessage(const QString &msg);

private slots:
    void onNewConnection();
    void onTextMessageReceived(const QString &message);
    void onClientDisconnected();

private:
    QWebSocketServer *m_server;
    QList<QWebSocket *> m_clients;
};

#endif // WEBSOCKETSERVER_H
