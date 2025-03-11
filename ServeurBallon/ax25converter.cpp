// AX25Converter.cpp
#include "ax25converter.h"

AX25Converter::AX25Converter(QObject *parent) : QObject(parent) { }

QByteArray AX25Converter::destAdrToAX25(const QString &indicatif) {
    QByteArray adr(7, 0x40);
    for (int i = 0; i < 6; i++) {
        char c = ' ';
        if (i < indicatif.size())
            c = indicatif.at(i).toLatin1();
        adr[i] = c << 1;
    }
    adr[6] = 0x60;
    return adr;
}

QByteArray AX25Converter::convertTNC2ToAX25(const QString &tnc2) {
    // (gérer la séparation source/destination/payload et construire la trame AX.25)
    // Vérifier si la chaîne est vide
    if (tnc2.isEmpty()) {
        return QByteArray();
    }

    // Exemple : "F4KMN-1>APIN21,WIDE1-1::F4KMN-8  :QSA?{10"
    int posDest    = tnc2.indexOf('>');
    int posPathSep = tnc2.indexOf(',', posDest);  // pour trouver la virgule
    int posPayload = tnc2.indexOf(':', posDest);  // pour trouver le 1er ':'

    if (posDest < 0 || posPayload < 0) {
        return QByteArray();
    }

    // Récupération de chaque champ
    // Source = de début jusqu'à '>'
    QString source = tnc2.left(posDest);

    // Destination (et éventuellement début du path)
    // ex: "APIN21,WIDE1-1"
    QString destPlusPath;
    if (posPathSep >= 0)
        destPlusPath = tnc2.mid(posDest + 1, posPathSep - (posDest + 1));
    else
        destPlusPath = tnc2.mid(posDest + 1, posPayload - (posDest + 1));

    // Si on a une virgule, on sépare la partie 'destination' de la partie 'path'
    QString destStr;
    QString pathStr;
    int commaPos = destPlusPath.indexOf(',');
    if (commaPos >= 0) {
        destStr = destPlusPath.left(commaPos);
        pathStr = destPlusPath.mid(commaPos + 1);
    } else {
        // Pas de virgule => tout dans destStr
        destStr = destPlusPath;
    }

    // Payload APRS
    QString payload = tnc2.mid(posPayload + 1);

    // Exemple minimal AX.25
    QByteArray ax25Frame;

    // 1) Destination (ex: APIN21) encodée AX.25
    //    Si tu veux vraiment prendre le contenu de destStr, utilise-le ici
    ax25Frame.append(destAdrToAX25(destStr.isEmpty() ? "APRS" : destStr));

    // 2) Source (ex: F4KMN-1) encodée AX.25
    ax25Frame.append(destAdrToAX25(source));

    // 3) Contrôle = 0x03 (UI-frame), PID = 0xF0
    ax25Frame.append((char)0x03);
    ax25Frame.append((char)0xF0);

    // 4) Ajout du message ASCII
    ax25Frame.append(payload.toLatin1());

    return ax25Frame;
}

QString AX25Converter::convertAX25ToTNC2(const QByteArray &ax25) {
    // On recherche la fin de la liste des adresses.
    int offset = 0;
    int addressCount = 0;
    // Les adresses AX.25 sont constituées de champs de 7 octets.
    // Le dernier champ d'adresse est identifié par le bit d'extension (LSB du 7ème octet à 1).
    while (offset + 7 <= ax25.size()) {
        QByteArray addrField = ax25.mid(offset, 7);
        addressCount++;
        // Si le bit d'extension est positionné, c'est le dernier champ d'adresse.
        if (addrField.at(6) & 0x01) {
            offset += 7;
            break;
        }
        offset += 7;
    }
    // Vérifier qu'il reste au moins 2 octets pour le contrôle et le PID.
    if (offset + 2 > ax25.size())
        return QString();

    // Récupération du contrôle et du PID (même si ce ne sont pas 0x03 et 0xF0)
    unsigned char control = static_cast<unsigned char>(ax25.at(offset));
    unsigned char pid = static_cast<unsigned char>(ax25.at(offset + 1));
    offset += 2; // Passer au payload

    QByteArray payload = ax25.mid(offset);

    // Pour la conversion TNC2, on suppose que les deux premiers champs d'adresse
    // correspondent respectivement à la destination et à la source.
    if (addressCount < 2)
        return QString();

    QByteArray dest = ax25.mid(0, 7);
    QByteArray src  = ax25.mid(7, 7);
    QString sDest = decodeAX25Address(dest);
    QString sSrc  = decodeAX25Address(src);

    // Reconstruire la trame TNC2 au format "SRC>DEST:payload"
    return QString("%1>%2:%3")
        .arg(sSrc)
        .arg(sDest)
        .arg(QString::fromLatin1(payload));
}

QString AX25Converter::decodeAX25Address(const QByteArray &addr7) {
    if (addr7.size() < 7) return QString();
    QByteArray callsign(6, ' ');
    for (int i = 0; i < 6; i++) {
        unsigned char c = (unsigned char)addr7.at(i);
        c >>= 1;
        callsign[i] = (char)c;
    }
    QString cs = QString::fromLatin1(callsign).trimmed();
    unsigned char ssidByte = (unsigned char)addr7.at(6);
    unsigned int rawSsid = (ssidByte >> 1) & 0x0F;
    return (rawSsid == 0) ? cs : QString("%1-%2").arg(cs).arg(rawSsid);
}
