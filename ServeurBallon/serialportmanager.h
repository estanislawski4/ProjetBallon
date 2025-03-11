#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

// SerialPortManager.h
#pragma once

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialPortManager : public QObject {
    Q_OBJECT
public:
    explicit SerialPortManager(QObject *parent = nullptr);
    QStringList availablePorts() const;
    bool openPort(const QString &portName, QString &errorString);
    qint64 writeData(const QByteArray &data);
    void closePort();

signals:
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &error);

private slots:
    void onReadyRead();
    void onErrorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort m_serial;
};

#endif // SERIALPORTMANAGER_H
