// KISSHandler.cpp
#include "kisshandler.h"
#include "aprsisclient.h"
#include "ax25converter.h"

KISSHandler::KISSHandler(APRSISClient *aprsClient, AX25Converter *converter, QObject *parent)
    : QObject(parent),
    m_aprsClient(aprsClient),
    m_converter(converter),
    m_sendToAprs(false)
{ }

void KISSHandler::setSendToAprs(bool enabled) {
    m_sendToAprs = enabled;
}

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

        // Extraction du source : tout avant le '>'
        int posGT = tnc2.indexOf('>');
        QString src = (posGT != -1) ? tnc2.left(posGT).trimmed() : "";

        // Extraction du destination :
        // Si la trame contient un double deux-points "::", on prend l'indicatif qui suit.
        int posDoubleColon = tnc2.lastIndexOf("::");
        QString dest;
        if (posDoubleColon != -1) {
            // On recherche le premier ':' après le double deux-points.
            int posColonAfter = tnc2.indexOf(':', posDoubleColon + 2);
            if (posColonAfter != -1) {
                dest = tnc2.mid(posDoubleColon + 2, posColonAfter - (posDoubleColon + 2)).trimmed();
            }
        } else {
            // Sinon, on utilise la partie entre '>' et le premier ':'
            int posColon = tnc2.indexOf(':', posGT + 1);
            if (posColon != -1) {
                dest = tnc2.mid(posGT + 1, posColon - posGT - 1).trimmed();
            }
        }

        // Extraction du message utile : tout ce qui suit le dernier ':'
        int posLastColon = tnc2.lastIndexOf(':');
        QString messageUtil = (posLastColon != -1) ? tnc2.mid(posLastColon + 1).trimmed() : "";

        // Émettre le signal avec les informations extraites
        emit loRaFrameReceived(src, dest, tnc2, messageUtil);

        // Si l'option est activée, envoyer sur aprs.fi
        if (m_sendToAprs) {
            m_aprsClient->sendLine(tnc2 + "\r\n");
            emit logMessage("Message APRS envoyé depuis la réception LoRa");
        }
    } else {
        emit logMessage("Impossible de convertir AX.25 -> TNC2 (pas UI frame?)");
    }
}
