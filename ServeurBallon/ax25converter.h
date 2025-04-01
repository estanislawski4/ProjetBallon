#ifndef AX25CONVERTER_H
#define AX25CONVERTER_H

/**
 * @file AX25Converter.h
 * @brief Déclaration de la classe AX25Converter.
 *
 * Ce fichier définit la classe AX25Converter, qui fournit des fonctions de conversion
 * entre le format TNC2 et la trame AX.25, ainsi que des outils pour décoder des adresses
 * au format AX.25.
 */

#pragma once

#include <QObject>
#include <QByteArray>
#include <QString>

/**
 * @brief Classe de conversion pour le protocole AX.25.
 *
 * La classe AX25Converter offre des méthodes pour convertir des trames TNC2 en trames AX.25,
 * pour convertir une trame AX.25 en format TNC2 et pour décoder les adresses AX.25.
 */
class AX25Converter : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructeur de la classe AX25Converter.
     * @param parent Pointeur vers l'objet parent (par défaut nullptr).
     */
    explicit AX25Converter(QObject *parent = nullptr);

    /**
     * @brief Convertit un indicatif en adresse AX.25.
     *
     * Transforme une chaîne de caractères représentant un indicatif (callsign)
     * en une adresse AX.25 codée sur 7 octets. Les 6 premiers octets contiennent
     * le callsign décalé d'un bit et le 7ème octet est configuré pour le SSID.
     *
     * @param indicatif L'indicatif à convertir.
     * @return QByteArray Représentation de l'adresse AX.25.
     */
    QByteArray destAdrToAX25(const QString &indicatif);

    /**
     * @brief Convertit une trame TNC2 en trame AX.25.
     *
     * Analyse la trame TNC2 afin d'extraire les adresses source et destination, ainsi que
     * la charge utile, puis construit une trame conforme au protocole AX.25.
     *
     * @param tnc2 La trame au format TNC2.
     * @return QByteArray La trame convertie au format AX.25, ou QByteArray() en cas d'erreur.
     */
    QByteArray convertTNC2ToAX25(const QString &tnc2);

    /**
     * @brief Convertit une trame AX.25 en format TNC2.
     *
     * Extrait les adresses source et destination, le contrôle, le PID et la charge utile
     * d'une trame AX.25 afin de la reformater au format TNC2.
     *
     * @param ax25 La trame au format AX.25.
     * @return QString La trame convertie au format TNC2, ou une chaîne vide en cas d'erreur.
     */
    QString convertAX25ToTNC2(const QByteArray &ax25);

    /**
     * @brief Décode une adresse AX.25 sur 7 octets.
     *
     * Extrait le callsign et le SSID d'une adresse AX.25 codée sur 7 octets et
     * retourne l'adresse sous forme de chaîne (par exemple "CALL" ou "CALL-SSID").
     *
     * @param addr7 La représentation en 7 octets de l'adresse AX.25.
     * @return QString L'adresse décodée sous forme de chaîne.
     */
    static QString decodeAX25Address(const QByteArray &addr7);
};

#endif // AX25CONVERTER_H
