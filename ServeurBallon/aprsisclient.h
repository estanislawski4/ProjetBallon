#ifndef APRSISCLIENT_H
#define APRSISCLIENT_H

// APRSISClient.h
#pragma once

#include <QObject>
#include <QTcpSocket>

class APRSISClient : public QObject {
    Q_OBJECT
public:
    explicit APRSISClient(QObject *parent = nullptr);
    void connectToServer(const QString &host, int port);
    void sendLine(const QString &line);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &msg);
    void errorOccurred(const QString &error);

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket m_socket;
    QString mIGateCall;
    QString mIGatePass;
};


#endif // APRSISCLIENT_H
