#ifndef APRSISCLIENT_H
#define APRSISCLIENT_H

/**
 * @file APRSISClient.h
 * @brief Déclaration de la classe APRSISClient.
 *
 * Ce fichier contient la déclaration de la classe APRSISClient qui permet de gérer
 * la communication avec un serveur APRS-IS via une connexion TCP. Il s'occupe notamment
 * de l'envoi des trames (login, messages) et de la réception des messages du serveur.
 */

#pragma once

#include <QObject>
#include <QTcpSocket>

/**
 * @brief Client pour la communication avec un serveur APRS-IS.
 *
 * La classe APRSISClient établit une connexion TCP avec un serveur APRS-IS, envoie
 * des lignes de trame (par exemple pour l'authentification ou l'envoi de messages)
 * et traite la réception des messages.
 */
class APRSISClient : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructeur de la classe APRSISClient.
     * @param parent Pointeur vers l'objet parent (par défaut nullptr).
     */
    explicit APRSISClient(QObject *parent = nullptr);

    /**
     * @brief Établit une connexion avec le serveur APRS-IS.
     * @param host Adresse du serveur.
     * @param port Port de connexion.
     */
    void connectToServer(const QString &host, int port);

    /**
     * @brief Envoie une ligne de trame vers le serveur APRS-IS.
     * @param line La ligne de trame à envoyer.
     */
    void sendLine(const QString &line);

signals:
    /**
     * @brief Signal émis lors de l'établissement de la connexion avec le serveur.
     */
    void connected();

    /**
     * @brief Signal émis lors de la déconnexion du serveur.
     */
    void disconnected();

    /**
     * @brief Signal émis lorsqu'un message est reçu du serveur APRS-IS.
     * @param msg Le message reçu.
     */
    void messageReceived(const QString &msg);

    /**
     * @brief Signal émis lorsqu'une erreur se produit.
     * @param error Description de l'erreur.
     */
    void errorOccurred(const QString &error);

private slots:
    /**
     * @brief Slot appelé lorsque la connexion au serveur est établie.
     *
     * Ce slot émet le signal @c connected et envoie la ligne de login pour s'authentifier.
     */
    void onConnected();

    /**
     * @brief Slot appelé lorsque des données sont disponibles en lecture.
     *
     * Lit les données disponibles sur le socket et émet le signal @c messageReceived.
     */
    void onReadyRead();

    /**
     * @brief Slot appelé lors de la déconnexion du serveur.
     *
     * Émet le signal @c disconnected pour notifier la perte de connexion.
     */
    void onDisconnected();

private:
    QTcpSocket m_socket;      ///< Socket TCP utilisé pour la communication avec le serveur APRS-IS.
    QString mIGateCall;       ///< Call sign utilisé pour la connexion (identifiant APRS-IS).
    QString mIGatePass;       ///< Mot de passe associé au call sign pour la connexion APRS-IS.
};

#endif // APRSISCLIENT_H
