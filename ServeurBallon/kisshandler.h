// kisshandler.h
#ifndef KISSHANDLER_H
#define KISSHANDLER_H

#include <QObject>

class APRSISClient;
class AX25Converter;

class KISSHandler : public QObject {
    Q_OBJECT
public:
    explicit KISSHandler(APRSISClient *aprsClient, AX25Converter *converter, QObject *parent = nullptr);
    void setSendToAprs(bool enabled);
    void parseKISSData(const QByteArray &data);

signals:
    void logMessage(const QString &msg);
    // Nouveau signal pour transmettre une trame LoRa reçue
    void loRaFrameReceived(const QString &source,
                           const QString &destination,
                           const QString &fullTrame,
                           const QString &message);

private:
    void processKISSFrame(const QByteArray &frame);
    APRSISClient *m_aprsClient;
    AX25Converter *m_converter;
    bool m_sendToAprs;
};

#endif // KISSHANDLER_H
