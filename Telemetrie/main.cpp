#include "telemetrie.h"       // Inclusion de l'en-tête de la classe Telemetrie pour l'accès aux mesures du BME280
#include "GestionFile.h"      // Inclusion de l'en-tête de la classe GestionFile pour gérer la file IPC
#include <thread>             // Inclusion de la bibliothèque pour la gestion des threads et des pauses

int main(int argc, char **argv)
{
    try{
        // Création d'un objet GestionTemps pour gérer les informations temporelles
        GestionTemps gestionTemps;

        // Création d'un objet Telemetrie pour gérer les mesures du capteur BME280
        Telemetrie telemetrie;

        // Création d'un objet GestionFile pour la gestion des communications inter-processus (IPC)
        GestionFile gestionFile;

        // Tentative d'obtention de la file IPC via le port 5679
        gestionFile.obtenirFileIPC(5679);

        // Boucle infinie pour exécuter les actions à intervalles réguliers
        while (true)
        {
            // Récupère l'heure actuelle sous forme d'un objet std::tm
            std::tm tmMaintenant = gestionTemps.obtenirHeureActuelle();

            // Sauvegarde les mesures dans le fichier CSV toutes les 10 secondes
            if (tmMaintenant.tm_sec % 10 == 0) {
                std::cout << gestionTemps.getDateFormatee(); // Affichage de la date formatée
                std::cout << " : > CSV "; // Message indiquant que l'on va sauvegarder les mesures dans le CSV
                telemetrie.SauvegarderEnCsv(); // Sauvegarde les mesures dans le fichier CSV
                std::cout <<std::endl <<" Température: " << telemetrie.getTemperature() << "°F" << std::endl;
                std::cout << " Pression: " << telemetrie.getPression() << std::endl;
                std::cout << " Humidité: " << telemetrie.getHumidite() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1)); // Attente d'une seconde pour éviter la répétition rapide des actions
            }

            // Envoie la trame APRS weather toutes les 2 minutes à la seconde 30
            if (tmMaintenant.tm_min % 2 == 0 && tmMaintenant.tm_sec == 30) {
                std::string trame = telemetrie.CreationTrameAPRS(); // Création de la trame APRS à partir des mesures
                // Tentative d'écriture de la trame dans la file IPC
                if (!gestionFile.ecrireDansLaFileIPC(trame)) {
                    // Si l'écriture échoue, on affiche un message d'erreur
                    std::cout << gestionTemps.getDateFormatee();
                    std::cout << " : >APLT : Erreur ecriture file" << std::endl;
                }
                else {
                    // Si l'écriture réussit, on affiche la trame envoyée
                    std::cout << gestionTemps.getDateFormatee();
                    std::cout << " : >APLT : ";
                    std::cout << trame << std::endl;
                }
            }
        }
    }
    catch (const std::exception &e) {
        // Si une exception est levée à un moment donné, on l'affiche
        std::cerr << "Exception attrapée: " << e.what() << std::endl;
    }
}
