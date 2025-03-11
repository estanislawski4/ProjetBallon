// SerialPortManager.cpp
#include "serialportmanager.h"
#include <QDebug>

SerialPortManager::SerialPortManager(QObject *parent)
    : QObject(parent)
{
    connect(&m_serial, &QSerialPort::readyRead, this, &SerialPortManager::onReadyRead);
    connect(&m_serial, &QSerialPort::errorOccurred, this, &SerialPortManager::onErrorOccurred);
}

QStringList SerialPortManager::availablePorts() const {
    QStringList ports;
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        ports << info.portName();
    }
    return ports;
}

bool SerialPortManager::openPort(const QString &portName, QString &errorString) {
    m_serial.setPortName(portName);
    m_serial.setBaudRate(QSerialPort::Baud9600);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serial.open(QIODevice::ReadWrite)) {
        errorString = m_serial.errorString();
        return false;
    }
    return true;
}

qint64 SerialPortManager::writeData(const QByteArray &data) {
    return m_serial.write(data);
}

void SerialPortManager::closePort() {
    if(m_serial.isOpen())
        m_serial.close();
}

void SerialPortManager::onReadyRead() {
    QByteArray data = m_serial.readAll();
    emit dataReceived(data);
}

void SerialPortManager::onErrorOccurred(QSerialPort::SerialPortError error) {
    if(error != QSerialPort::NoError)
        emit errorOccurred(m_serial.errorString());
}
