#include "ax25converter.h"

/**
 * @file ax25converter.cpp
 * @brief Constructeur de la classe AX25Converter.
 * @author Erwän STANISLAWSKI
 *
 * Initialise un objet AX25Converter sans effectuer d'opération particulière.
 *
 * @param parent Pointeur vers l'objet parent (par défaut nullptr).
 */
AX25Converter::AX25Converter(QObject *parent)
    : QObject(parent)
{ }

/**
 * @brief Convertit un indicatif en adresse AX.25.
 *
 * Convertit une chaîne représentant un indicatif en une adresse AX.25 codée sur 7 octets.
 * Les 6 premiers octets contiennent le callsign décalé d'un bit vers la gauche,
 * et le 7ème octet est configuré pour le SSID.
 *
 * @param indicatif L'indicatif (callsign) à convertir.
 * @return QByteArray Représentation de l'adresse au format AX.25.
 */
QByteArray AX25Converter::destAdrToAX25(const QString &indicatif)
{
    QByteArray adr(7, 0x40); // Initialisation avec des espaces (0x40)
    for (int i = 0; i < 6; ++i) {
        char c = ' ';
        if (i < indicatif.size())
            c = indicatif.at(i).toLatin1();
        adr[i] = c << 1;
    }
    adr[6] = 0x60; // Configuration du SSID
    return adr;
}

/**
 * @brief Convertit une trame TNC2 en trame AX.25.
 *
 * Analyse la trame TNC2 pour extraire les adresses source et destination ainsi que la charge utile,
 * puis construit une trame conforme au protocole AX.25 en utilisant les méthodes de conversion d'adresse.
 *
 * @param tnc2 La trame au format TNC2.
 * @return QByteArray La trame AX.25 convertie, ou QByteArray() en cas d'erreur.
 */
QByteArray AX25Converter::convertTNC2ToAX25(const QString &tnc2)
{
    if (tnc2.isEmpty())
        return QByteArray();

    int posDest    = tnc2.indexOf('>');
    int posPathSep = tnc2.indexOf(',', posDest);
    int posPayload = tnc2.indexOf(':', posDest);

    if (posDest < 0 || posPayload < 0)
        return QByteArray();

    // Extraction des champs source et destination
    QString source = tnc2.left(posDest);
    QString destPlusPath = (posPathSep >= 0)
                               ? tnc2.mid(posDest + 1, posPathSep - (posDest + 1))
                               : tnc2.mid(posDest + 1, posPayload - (posDest + 1));

    QString destStr;
    int commaPos = destPlusPath.indexOf(',');
    if (commaPos >= 0) {
        destStr = destPlusPath.left(commaPos);
    } else {
        destStr = destPlusPath;
    }

    QString payload = tnc2.mid(posPayload + 1);
    QByteArray ax25Frame;

    // Construire la trame AX.25
    ax25Frame.append(destAdrToAX25(destStr.isEmpty() ? "APRS" : destStr)); // Destination
    ax25Frame.append(destAdrToAX25(source));                                // Source
    ax25Frame.append((char)0x03); // Contrôle (UI-frame)
    ax25Frame.append((char)0xF0); // PID
    ax25Frame.append(payload.toLatin1()); // Payload

    return ax25Frame;
}

/**
 * @brief Convertit une trame AX.25 en format TNC2.
 *
 * Parcourt la trame AX.25 pour extraire les champs d'adresses, contrôle, PID et la charge utile,
 * puis reconstruit une trame au format TNC2 en décodant les adresses via la méthode decodeAX25Address.
 *
 * @param ax25 La trame au format AX.25.
 * @return QString La trame convertie au format TNC2, ou une chaîne vide en cas d'erreur.
 */
QString AX25Converter::convertAX25ToTNC2(const QByteArray &ax25)
{
    int offset = 0;
    int addressCount = 0;
    // Parcours des champs d'adresses (7 octets chacun)
    while (offset + 7 <= ax25.size()) {
        QByteArray addrField = ax25.mid(offset, 7);
        ++addressCount;
        // Vérifier le bit d'extension pour identifier le dernier champ
        if (addrField.at(6) & 0x01) {
            offset += 7;
            break;
        }
        offset += 7;
    }
    if (offset + 2 > ax25.size())
        return QString();

    // Récupérer contrôle et PID (même s'ils ne sont pas standard)
    offset += 2; // Passer contrôle et PID
    QByteArray payload = ax25.mid(offset);

    if (addressCount < 2)
        return QString();

    QByteArray dest = ax25.mid(0, 7);
    QByteArray src  = ax25.mid(7, 7);
    QString sDest = decodeAX25Address(dest);
    QString sSrc  = decodeAX25Address(src);

    return QString("%1>%2:%3")
        .arg(sSrc)
        .arg(sDest)
        .arg(QString::fromLatin1(payload));
}

/**
 * @brief Décode une adresse AX.25 sur 7 octets.
 *
 * Extrait le callsign des 6 premiers octets en décalant chaque octet d'un bit vers la droite,
 * et récupère le SSID à partir du 7ème octet. Le SSID est concaténé au callsign si différent de 0.
 *
 * @param addr7 La représentation en 7 octets de l'adresse AX.25.
 * @return QString L'adresse décodée sous forme de chaîne, par exemple "CALL-SSID" ou "CALL".
 */
QString AX25Converter::decodeAX25Address(const QByteArray &addr7)
{
    if (addr7.size() < 7)
        return QString();

    QByteArray callsign(6, ' ');
    for (int i = 0; i < 6; ++i) {
        unsigned char c = static_cast<unsigned char>(addr7.at(i));
        c >>= 1;
        callsign[i] = static_cast<char>(c);
    }
    QString cs = QString::fromLatin1(callsign).trimmed();
    unsigned char ssidByte = static_cast<unsigned char>(addr7.at(6));
    unsigned int rawSsid = (ssidByte >> 1) & 0x0F;
    return (rawSsid == 0) ? cs : QString("%1-%2").arg(cs).arg(rawSsid);
}
