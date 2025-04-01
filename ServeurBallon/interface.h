#ifndef INTERFACE_H
#define INTERFACE_H

/**
 * @file Interface.h
 * @brief Déclaration de la classe Interface.
 *
 * Ce fichier définit la classe Interface qui fournit l'interface graphique permettant de
 * gérer l'envoi et le stockage des trames APRS et LoRa. Elle orchestre la communication entre
 * différents gestionnaires (SerialPortManager, APRSISClient, AX25Converter, KISSHandler, MySQLManager).
 */

#include <QWidget>

namespace Ui {
class Interface;
}

/**
 * @brief Interface utilisateur pour la gestion des trames APRS et LoRa.
 *
 * La classe Interface gère l'interaction avec l'utilisateur pour la sélection des ports série,
 * l'envoi des trames APRS et LoRa, ainsi que le stockage de ces dernières en base de données.
 */

class SerialPortManager;
class APRSISClient;
class AX25Converter;
class KISSHandler;
class WebSocketServer;
class MySQLManager;

class Interface : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur de la classe Interface.
     * @param parent Pointeur vers l'objet parent (par défaut nullptr).
     */
    explicit Interface(QWidget *parent = nullptr);

    /**
     * @brief Destructeur de la classe Interface.
     *
     * Libère les ressources allouées et ferme la connexion à la base de données si nécessaire.
     */
    ~Interface();

private slots:
    /**
     * @brief Remplit la combobox avec les ports série disponibles.
     *
     * Interroge le gestionnaire SerialPortManager et met à jour l'interface avec la liste
     * des ports détectés.
     */
    void fillPortsComboBox();

    /**
     * @brief Gère l'action du bouton "Start".
     *
     * Tente d'ouvrir le port série sélectionné et affiche le résultat de l'opération.
     */
    void onStartButtonClicked();

    /**
     * @brief Gère l'envoi d'une trame.
     *
     * Construit et envoie une trame APRS et une trame LoRa, effectue leur conversion et
     * leur stockage dans la base de données. Les messages de statut sont affichés dans le log.
     */
    void onSendButtonClicked();

private:
    Ui::Interface *ui;                   ///< Pointeur vers l'interface utilisateur générée par Qt Designer.
    SerialPortManager *m_serialManager;  ///< Gestionnaire des communications série.
    APRSISClient     *m_aprsClient;       ///< Client pour la communication avec le serveur APRS-IS.
    AX25Converter    *m_converter;        ///< Outil de conversion entre les formats TNC2 et AX.25.
    KISSHandler      *m_kissHandler;      ///< Gestionnaire pour le protocole KISS.
    MySQLManager     *m_dbManager;        ///< Gestionnaire de la base de données MySQL.

    /**
     * @brief Construit une trame LoRa au format TNC2.
     *
     * Utilise les informations saisies dans l'interface pour créer une trame LoRa.
     *
     * @return QString La trame LoRa construite.
     */
    QString buildLoRaFrame();

    /**
     * @brief Construit une trame APRS.
     *
     * Utilise les informations saisies dans l'interface pour créer une trame APRS.
     *
     * @return QString La trame APRS construite.
     */
    QString buildAprsFrame();

    /**
     * @brief Stocke une trame LoRa dans la base de données.
     *
     * Vérifie l'existence des machines source et destination, insère celles-ci si nécessaire,
     * puis insère la trame et son message associé dans la base.
     *
     * @param source Indicatif de la machine source.
     * @param destination Indicatif de la machine destination.
     * @param fullTrame La trame complète à stocker.
     * @param message Le message extrait de la trame.
     * @return bool @c true si le stockage a réussi, @c false sinon.
     */
    bool storeLoRaTrame(const QString &source, const QString &destination,
                        const QString &fullTrame, const QString &message);
};

#endif // INTERFACE_H
