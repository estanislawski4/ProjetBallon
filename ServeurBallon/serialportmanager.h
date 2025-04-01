#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

/**
 * @file SerialPortManager.h
 * @brief Déclaration de la classe SerialPortManager.
 *
 * Ce fichier déclare la classe SerialPortManager qui permet de gérer la communication via un port série
 * en utilisant QSerialPort. Elle offre des méthodes pour lister les ports disponibles, ouvrir et fermer
 * un port, écrire des données, et émet des signaux pour notifier la réception de données ou d'erreurs.
 */

#pragma once

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

/**
 * @brief Gestionnaire de communication série.
 *
 * La classe SerialPortManager encapsule les fonctionnalités de QSerialPort pour faciliter l'accès
 * aux ports série disponibles, la configuration et la gestion des communications série.
 */
class SerialPortManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructeur de la classe SerialPortManager.
     *
     * Initialise l'objet SerialPortManager et connecte les signaux du port série aux slots appropriés.
     *
     * @param parent Pointeur vers l'objet parent (par défaut nullptr).
     */
    explicit SerialPortManager(QObject *parent = nullptr);

    /**
     * @brief Retourne la liste des ports série disponibles.
     *
     * Parcourt la liste des ports accessibles via QSerialPortInfo et retourne leurs noms.
     *
     * @return QStringList Liste des noms de ports disponibles.
     */
    QStringList availablePorts() const;

    /**
     * @brief Ouvre le port série spécifié.
     *
     * Configure le port avec les paramètres par défaut (9600 bauds, 8 bits, pas de parité,
     * 1 bit d'arrêt, pas de contrôle de flux) et tente de l'ouvrir en mode lecture/écriture.
     *
     * @param portName Nom du port à ouvrir.
     * @param errorString Référence à une chaîne pour retourner le message d'erreur en cas d'échec.
     * @return bool @c true si le port est ouvert avec succès, @c false sinon.
     */
    bool openPort(const QString &portName, QString &errorString);

    /**
     * @brief Écrit des données sur le port série.
     *
     * Envoie un tableau d'octets sur le port série ouvert.
     *
     * @param data Les données à écrire.
     * @return qint64 Le nombre d'octets écrits, ou une valeur négative en cas d'erreur.
     */
    qint64 writeData(const QByteArray &data);

    /**
     * @brief Ferme le port série.
     *
     * Ferme le port si celui-ci est actuellement ouvert.
     */
    void closePort();

signals:
    /**
     * @brief Signal émis lors de la réception de données sur le port série.
     *
     * Ce signal est émis dès qu'un bloc de données est disponible sur le port.
     *
     * @param data Les données reçues.
     */
    void dataReceived(const QByteArray &data);

    /**
     * @brief Signal émis en cas d'erreur sur le port série.
     *
     * Ce signal est émis lorsqu'une erreur se produit pendant la communication série.
     *
     * @param error Description de l'erreur.
     */
    void errorOccurred(const QString &error);

private slots:
    /**
     * @brief Slot appelé lorsque des données sont prêtes à être lues.
     *
     * Lit toutes les données disponibles sur le port série et émet le signal @c dataReceived.
     */
    void onReadyRead();

    /**
     * @brief Slot appelé en cas d'erreur sur le port série.
     *
     * Vérifie le type d'erreur et, si l'erreur n'est pas QSerialPort::NoError, émet le signal @c errorOccurred.
     *
     * @param error Code d'erreur retourné par QSerialPort.
     */
    void onErrorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort m_serial; ///< Objet QSerialPort utilisé pour la communication série.
};

#endif // SERIALPORTMANAGER_H
