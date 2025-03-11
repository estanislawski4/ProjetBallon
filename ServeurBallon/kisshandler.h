#ifndef KISSHANDLER_H
#define KISSHANDLER_H

// KISSHandler.h
#pragma once

#include <QObject>
#include <QByteArray>

class APRSISClient; // Déclaration anticipée
class AX25Converter;

class KISSHandler : public QObject {
    Q_OBJECT
public:
    explicit KISSHandler(APRSISClient *aprsClient, AX25Converter *converter, QObject *parent = nullptr);
    void parseKISSData(const QByteArray &data);

signals:
    void logMessage(const QString &msg);

private:
    void processKISSFrame(const QByteArray &frame);
    APRSISClient *m_aprsClient;
    AX25Converter *m_converter;
};

#endif // KISSHANDLER_H
