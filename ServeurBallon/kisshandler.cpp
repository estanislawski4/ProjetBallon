// KISSHandler.cpp
#include "kisshandler.h"
#include "aprsisclient.h"
#include "ax25converter.h"

KISSHandler::KISSHandler(APRSISClient *aprsClient, AX25Converter *converter, QObject *parent)
    : QObject(parent),
    m_aprsClient(aprsClient),
    m_converter(converter)
{ }

void KISSHandler::parseKISSData(const QByteArray &data) {
    static QByteArray frameBuffer;
    static bool inFrame = false;
    static bool inEscape = false;

    for (unsigned char c : data) {
        if (c == 0xC0) {
            if (inFrame) {
                if (!frameBuffer.isEmpty())
                    processKISSFrame(frameBuffer);
                frameBuffer.clear();
                inFrame = false;
            } else {
                frameBuffer.clear();
                inFrame = true;
            }
            inEscape = false;
        } else if (!inFrame) {
            continue;
        } else if (c == 0xDB) {
            inEscape = true;
        } else if (inEscape) {
            frameBuffer.append((char)((c == 0xDC) ? 0xC0 : (c == 0xDD) ? 0xDB : c));
            inEscape = false;
        } else {
            frameBuffer.append((char)c);
        }
    }
}

void KISSHandler::processKISSFrame(const QByteArray &frame) {
    if (frame.isEmpty()) return;
    unsigned char portByte = static_cast<unsigned char>(frame.at(0));
    QByteArray ax25Payload = frame.mid(1);
    emit logMessage(QString("Réception KISS => Port: 0x%1, Payload(hex): %2")
                        .arg(portByte, 2, 16, QLatin1Char('0'))
                        .arg(QString(ax25Payload.toHex(' '))));

    QString tnc2 = m_converter->convertAX25ToTNC2(ax25Payload);
    if (!tnc2.isEmpty()) {
        emit logMessage("Trame convertie => " + tnc2);
        m_aprsClient->sendLine(tnc2 + "\r\n");
    } else {
        emit logMessage("Impossible de convertir AX.25 -> TNC2 (pas UI frame?)");
    }
}
