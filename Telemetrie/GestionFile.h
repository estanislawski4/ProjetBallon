/**
 * @file GestionFile.h
 * @brief Classe qui gère les files IPC (Inter Process Communication).
 *
 * Cette classe permet de créer, écrire et lire des messages dans une file de messages IPC.
 * Elle utilise les bibliothèques standards C++ et les appels système Unix pour gérer la file.
 */

#ifndef GESTIONFILE_H
#define GESTIONFILE_H

#include <string>    ///< Inclut la bibliothèque standard de C++ pour les chaînes de caractères
#include <mutex>     ///< Inclut la bibliothèque standard de C++ pour les mutex
#include <stdexcept> ///< Inclut la bibliothèque standard de C++ pour les exceptions
#include <cstring>   ///< Inclut la bibliothèque standard de C pour strncpy
#include <sys/ipc.h> ///< Inclut la bibliothèque de système pour IPC_CREAT
#include <sys/msg.h> ///< Inclut la bibliothèque de système pour les files de messages

/**
 * @brief Taille maximum du message dans la file.
 */
const int MAX_MESSAGE_SIZE = 256;

/**
 * @brief Flag pour la création de la file de messages.
 * Définit les permissions d'accès et active la création de la file si elle n'existe pas.
 */
const int MSG_FLAG = 0666 | IPC_CREAT; // Flag de création de la file de messages


/**
 * @struct MessageTX
 * @brief Structure pour le message de transmission (TX).
 *
 * Cette structure est utilisée pour stocker les messages envoyés dans la file IPC.
 */
struct MessageTX
{
    long type;
    char text[MAX_MESSAGE_SIZE];
};

/**
 * @struct MessageRX
 * @brief Structure pour le message de réception (RX).
 *
 * Cette structure est utilisée pour stocker les messages reçus de la file IPC.
 */
struct MessageRX
{
    long  type;
    char  text[MAX_MESSAGE_SIZE];
    float SNR;
    int   RSSI;
};

/**
 * @class GestionFile
 * @brief Classe pour la gestion des files IPC.
 *
 * Cette classe permet de gérer les files de messages IPC, permettant ainsi l'échange de données
 * entre différents processus. Elle offre des méthodes pour créer une file IPC, écrire dans la file,
 * et lire les messages à partir de la file.
 */
class GestionFile
{
private:
    std::mutex mutexTx; ///< Mutex pour la synchronisation lors de l'écriture dans la file
    int fileId; ///< Identifiant de la file IPC

public:
    /**
     * @brief Constructeur de la classe GestionFile.
     *
     * Initialise l'objet GestionFile et prépare la file IPC.
     */
    GestionFile();

    /**
     * @brief Destructeur de la classe GestionFile.
     *
     * Libère la file IPC lorsque l'objet est détruit.
     */
    ~GestionFile();

    /**
     * @brief Ouvre la file IPC avec une clé spécifique.
     *
     * @param key Clé unique permettant d'identifier la file IPC.
     * @throws std::runtime_error Si l'ouverture de la file échoue.
     */
    void obtenirFileIPC(const int key);

    /**
     * @brief Écrit un message dans la file IPC.
     *
     * @param payload Le message à envoyer dans la file.
     * @return true Si le message a été écrit avec succès.
     * @return false Si l'écriture dans la file échoue.
     */
    bool ecrireDansLaFileIPC(const std::string &payload);

    /**
     * @brief Lit un message dans la file IPC.
     *
     * @param type Le type du message à lire.
     * @return MessageRX Le message reçu dans la file IPC.
     * @throws std::runtime_error Si la lecture échoue.
     */
    MessageRX lireDansLaFileIPC(int type);
};

#endif // GESTIONFILE_H
