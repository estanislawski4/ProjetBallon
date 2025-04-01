#include "kisshandler.h"
#include "aprsisclient.h"
#include "ax25converter.h"

/**
 * @file KISSHandler.cpp
 * @brief Implémentation de la classe KISSHandler.
 *
 * Ce fichier contient l'implémentation des méthodes de la classe KISSHandler, qui gère
 * la réception et le décodage des données au format KISS, leur conversion en trame TNC2,
 * et la transmission vers APRS-IS ou la notification de réception de trames LoRa.
 */

/**
 * @brief Constructeur de la classe KISSHandler.
 *
 * Initialise l'objet KISSHandler en associant le client APRSISClient et le convertisseur AX25Converter,
 * et en désactivant par défaut l'envoi des trames vers APRS-IS.
 *
 * @param aprsClient Pointeur vers le client APRSISClient.
 * @param converter Pointeur vers l'objet AX25Converter.
 * @param parent Pointeur vers l'objet parent (par défaut nullptr).
 */
KISSHandler::KISSHandler(APRSISClient *aprsClient, AX25Converter *converter, QObject *parent)
    : QObject(parent),
    m_aprsClient(aprsClient),
    m_converter(converter),
    m_sendToAprs(false)
{ }

/**
 * @brief Active ou désactive l'envoi des trames converties vers APRS-IS.
 *
 * Met à jour l'état interne du gestionnaire pour déterminer si les trames converties
 * doivent être envoyées automatiquement via le client APRSISClient.
 *
 * @param enabled @c true pour activer l'envoi, @c false pour le désactiver.
 */
void KISSHandler::setSendToAprs(bool enabled)
{
    m_sendToAprs = enabled;
}

/**
 * @brief Analyse et décode des données reçues au format KISS.
 *
 * Parcourt le flux de données, gère les délimiteurs de trame (0xC0),
 * les séquences d'échappement (0xDB, 0xDC, 0xDD), et reconstitue les trames complètes.
 * Lorsqu'une trame complète est reconstituée, elle est transmise à la fonction processKISSFrame.
 *
 * @param data Les données brutes au format KISS.
 */
void KISSHandler::parseKISSData(const QByteArray &data)
{
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

/**
 * @brief Traite une trame KISS complète.
 *
 * Extrait le byte de port et le payload AX.25 de la trame, convertit ce dernier en format TNC2
 * via l'objet AX25Converter, et émet des signaux pour la journalisation. Si la conversion est réussie,
 * la trame TNC2 est également émise via le signal loRaFrameReceived et, si activé, envoyée vers APRS-IS.
 *
 * @param frame La trame KISS complète à traiter.
 */
void KISSHandler::processKISSFrame(const QByteArray &frame)
{
    if (frame.isEmpty())
        return;

    // Récupérer le byte de port et le payload AX.25
    unsigned char portByte = static_cast<unsigned char>(frame.at(0));
    QByteArray ax25Payload = frame.mid(1);

    emit logMessage(QString("Réception KISS => Port: 0x%1, Payload(hex): %2")
                        .arg(portByte, 2, 16, QLatin1Char('0'))
                        .arg(QString(ax25Payload.toHex(' '))));

    // Conversion de la trame AX.25 en format TNC2
    QString tnc2 = m_converter->convertAX25ToTNC2(ax25Payload);
    if (!tnc2.isEmpty()) {
        emit logMessage("Trame convertie => " + tnc2);

        // Extraction des informations source, destination et message
        int posGT = tnc2.indexOf('>');
        QString src = (posGT != -1) ? tnc2.left(posGT).trimmed() : "";
        QString dest;
        QString messageUtil;

        int posDoubleColon = tnc2.indexOf("::");
        if (posDoubleColon != -1) {
            int posColonAfter = tnc2.indexOf(':', posDoubleColon + 2);
            if (posColonAfter != -1) {
                dest = tnc2.mid(posDoubleColon + 2, posColonAfter - (posDoubleColon + 2)).trimmed();
                messageUtil = tnc2.mid(posColonAfter + 1).trimmed();
            }
        } else {
            int posColon = tnc2.indexOf(':', posGT + 1);
            if (posColon != -1) {
                dest = tnc2.mid(posGT + 1, posColon - posGT - 1).trimmed();
                messageUtil = tnc2.mid(posColon + 1).trimmed();
            }
        }

        emit loRaFrameReceived(src, dest, tnc2, messageUtil);

        // Envoi vers APRS-IS si activé
        if (m_sendToAprs) {
            m_aprsClient->sendLine(tnc2 + "\r\n");
            emit logMessage("Message APRS envoyé depuis la réception LoRa");
        }
    } else {
        emit logMessage("Impossible de convertir AX.25 -> TNC2 (pas UI frame?)");
    }
}
