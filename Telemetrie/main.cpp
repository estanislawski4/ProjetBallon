#include "telemetrie.h"
#include "GestionFile.h"
#include <thread>

int main(int argc, char **argv)
{
    try{
        GestionTemps gestionTemps; // Objet pour la gestion du temps
        Telemetrie telemetrie; // Objet pour la télémétrie des mesures
        GestionFile gestionFile; // Objet pour la gestion des files
        gestionFile.obtenirFileIPC(5679);
        while (true)
        {
            //Sauvegarde des mesures dans le fichier CSV toute les 10 secondes
            std::tm tmMaintenant = gestionTemps.obtenirHeureActuelle();
            if(tmMaintenant.tm_sec % 10 == 0){
                std::cout << gestionTemps.getDateFormatee();
                std::cout << " : > CSV ";
                telemetrie.SauvegarderEnCsv();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            //Envoie de la trame APRS weather toute les 2 minutes à la seconde 30
            if (tmMaintenant.tm_min % 2 == 0 && tmMaintenant.tm_sec == 30)
            {
                std::string trame = telemetrie.CreationTrameAPRS();
                if (!gestionFile.ecrireDansLaFileIPC(trame))
                {
                    std::cout << gestionTemps.getDateFormatee();
                    std::cout << " : >APLT : Erreur ecriture file" << std::endl;
                }
                else
                {
                    std::cout << gestionTemps.getDateFormatee();
                    std::cout << " : >APLT : ";
                    std::cout << trame << std::endl;
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception attrapée: " << e.what() << std::endl;
    }
}

