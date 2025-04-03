#include "GestionFile.h"
#include <iostream>

//        constructeur
GestionFile::GestionFile()
{
}
//        Destructeur
GestionFile::~GestionFile()
{
}

/**
 * @brief Ouvre une file de messages IPC avec la clé spécifiée.
 *
 * @param key Clé permettant d'identifier la file de messages.
 * @throws std::runtime_error si l'ouverture échoue.
 */

void GestionFile::obtenirFileIPC(const int key)
{
    // Essaye d'accéder à une file de messages existante avec la clé donnée.
    if ((fileId = msgget((key_t)key, MSG_FLAG)) == -1)
    {
        // Si l'accès échoue, une exception est levée pour signaler l'erreur.
        throw std::runtime_error("Erreur lors de l'ouverture de la file de messages.");
    }
}

/**
 * @brief Écrit un message dans la file de messages IPC.
 *
 * @param payload Le texte du message à envoyer.
 * @return true si l'envoi est réussi, false sinon.
 * @throws std::runtime_error en cas d'échec.
 */

bool GestionFile::ecrireDansLaFileIPC(const std::string &payload)
{
    // Création d'un verrou pour assurer la synchronisation (éviter les accès concurrents)
    std::lock_guard<std::mutex> lock(mutexTx);

    // Création d'un message de type `MessageTX`
    MessageTX message;
    message.type = 2; // On définit un type arbitraire pour le message

    // Copie du texte dans le message (attention à la taille maximale !)
    std::strcpy(message.text, payload.c_str());

    // Envoi du message dans la file
    if (msgsnd(fileId, &message, sizeof(message.text), 0) == -1)
    {
        // Si l'envoi échoue, une exception est levée.
        throw std::runtime_error("Erreur ecriture fileTX !");
        return false;
    }
    return true;
}

/**
 * @brief Lit un message dans la file de messages IPC en fonction du type.
 *
 * @param type Type de message attendu.
 * @return MessageRX contenant les données reçues.
 * @throws std::runtime_error en cas d'échec de lecture.
 */

 MessageRX GestionFile::lireDansLaFileIPC(int type){

    MessageRX message;// Objet pour stocker le message reçu
    int ret;

    // Lecture d'un message de la file IPC correspondant au type demandé
    ret = msgrcv(fileId, &message, sizeof(message) - 4, type, 0);
    if (ret == -1){
        // Si la lecture échoue, une exception est levée.
        throw std::runtime_error("Erreur lecture fileRX !");
    }   
    return message; // Retourne le message reçu
}
