#ifndef KISSHANDLER_H
#define KISSHANDLER_H

/**
 * @file kisshandler.h
 * @brief Déclaration de la classe KISSHandler.
 *
 * Ce fichier définit la classe KISSHandler qui gère l'interprétation des données
 * au format KISS. Elle se charge de décoder les trames KISS, de convertir les données
 * en trames TNC2 via un convertisseur AX.25 et d'émettre des signaux pour la journalisation
 * ou la transmission vers APRS-IS et la gestion des trames LoRa.
 */

#include <QObject>

class APRSISClient;
class AX25Converter;

/**
 * @brief Gestionnaire de trames KISS.
 *
 * La classe KISSHandler permet de traiter les données brutes reçues au format KISS.
 * Elle décode ces données pour extraire une trame AX.25, la convertit en format TNC2,
 * et transmet des informations via des signaux, notamment pour l'envoi de trames vers APRS-IS
 * et pour la notification de réception de trames LoRa.
 */
class KISSHandler : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructeur de la classe KISSHandler.
     *
     * Initialise le gestionnaire KISS avec les objets nécessaires à la communication APRS-IS
     * et à la conversion des trames.
     *
     * @param aprsClient Pointeur vers le client APRSISClient pour l'envoi des trames APRS.
     * @param converter Pointeur vers l'objet AX25Converter pour la conversion des trames.
     * @param parent Pointeur vers l'objet parent (par défaut nullptr).
     */
    explicit KISSHandler(APRSISClient *aprsClient, AX25Converter *converter, QObject *parent = nullptr);

    /**
     * @brief Active ou désactive l'envoi des trames converties vers APRS-IS.
     *
     * Permet de configurer le gestionnaire afin d'envoyer automatiquement les trames
     * converties vers le serveur APRS-IS.
     *
     * @param enabled @c true pour activer l'envoi, @c false pour le désactiver.
     */
    void setSendToAprs(bool enabled);

    /**
     * @brief Analyse et décode des données reçues au format KISS.
     *
     * Traite le flux de données KISS en découpant les trames, en gérant les séquences d'échappement
     * et en transmettant chaque trame complète pour traitement.
     *
     * @param data Les données brutes au format KISS.
     */
    void parseKISSData(const QByteArray &data);

signals:
    /**
     * @brief Signal pour la journalisation des messages.
     *
     * Émis pour notifier l'interface ou le système de log de messages d'information.
     *
     * @param msg Le message à journaliser.
     */
    void logMessage(const QString &msg);

    /**
     * @brief Signal pour la réception d'une trame LoRa.
     *
     * Émis lorsqu'une trame LoRa a été extraite et convertie depuis une trame KISS.
     *
     * @param source L'indicatif source de la trame.
     * @param destination L'indicatif destination de la trame.
     * @param fullTrame La trame complète au format TNC2.
     * @param message Le message extrait de la trame.
     */
    void loRaFrameReceived(const QString &source,
                           const QString &destination,
                           const QString &fullTrame,
                           const QString &message);

private:
    /**
     * @brief Traite une trame KISS complète.
     *
     * Décode la trame KISS en extrayant le port et le payload, convertit la trame AX.25 en format TNC2,
     * puis émet des signaux pour la journalisation et, si activé, pour l'envoi vers APRS-IS.
     *
     * @param frame La trame KISS complète à traiter.
     */
    void processKISSFrame(const QByteArray &frame);

    APRSISClient *m_aprsClient;     ///< Pointeur vers le client APRSISClient pour l'envoi de trames APRS.
    AX25Converter *m_converter;      ///< Pointeur vers l'objet AX25Converter pour la conversion des trames.
    bool m_sendToAprs;               ///< Indique si les trames converties doivent être envoyées vers APRS-IS.
};

#endif // KISSHANDLER_H
