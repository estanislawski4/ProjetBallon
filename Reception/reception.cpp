#include "GestionFile.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <cstring>   // pour les char*
#include <cctype>    // pour std::isalnum

// Petite fonction utilitaire pour "trim" (enlever les espaces en fin de chaîne, par ex.)
static void rtrim(std::string &s)
{
    while (!s.empty() && std::isspace((unsigned char)s.back())) {
        s.pop_back();
    }
}

// Enum pour lister les commandes Q gérées
enum class QCode {
    NONE,
    QSA,
    QTR,
    QSL
};

// Détermine quel code Q est présent dans la trame, s'il y en a un.
QCode parseQCode(const std::string &txt)
{
    if (txt.find("QSA?") != std::string::npos) {
        return QCode::QSA;
    }
    else if (txt.find("QTR?") != std::string::npos) {
        return QCode::QTR;
    }
    else if (txt.find("QSL?") != std::string::npos) {
        return QCode::QSL;
    }

    return QCode::NONE; // Par défaut, pas de code géré
}

int main(int argc, char **argv)
{
    try
    {
        GestionFile fileRX;           // Objet pour la gestion des files de messages
        GestionFile fileTX;

        fileRX.obtenirFileIPC(5678);  // Obtenir la file pour la réception, key 5678
        fileTX.obtenirFileIPC(5679);  // Obtenir la file pour l'émission, key 5679

        MessageRX message;
        int idMessage = 1;

        while (true)
        {
            message = fileRX.lireDansLaFileIPC(2);
            std::string txt(message.text); // Convertir le char[] en std::string
            std::cout << "Trame reçue: " << txt << std::endl;

            // 1) Extraire l'indicatif source (entre < et >)
            std::string sourceCallsign;
            {
                auto posStart = txt.find('<');
                auto posEnd   = txt.find('>', (posStart != std::string::npos ? posStart + 1 : 0));
                if (posStart != std::string::npos && posEnd != std::string::npos && posEnd > posStart)
                {
                    sourceCallsign = txt.substr(posStart + 1, posEnd - (posStart + 1));

                    while (!sourceCallsign.empty() && !std::isalnum((unsigned char)sourceCallsign[0])) {
                        sourceCallsign.erase(sourceCallsign.begin());
                    }
                }
            }

            // 2) Extraire l'indicatif destination (à partir de "::" jusqu'au prochain ':')
            std::string destCallsign;
            {
                auto posDoubleColon = txt.find("::");
                if (posDoubleColon != std::string::npos)
                {
                    auto posMsg = txt.find(":", posDoubleColon + 2);
                    if (posMsg != std::string::npos && posMsg > posDoubleColon)
                    {
                        destCallsign = txt.substr(posDoubleColon + 2, posMsg - (posDoubleColon + 2));
                        rtrim(destCallsign);
                    }
                }
            }

            // 3) Vérifier la validité de la source (commence par "F4KMN" ou "F4LTZ")
            bool sourceOK = false;
            if (!sourceCallsign.empty())
            {
                const std::string prefix1 = "F4KMN";
                const std::string prefix2 = "F4LTZ";
                if (sourceCallsign.size() >= prefix1.size())
                {
                    if (sourceCallsign.compare(0, prefix1.size(), prefix1) == 0 ||
                        sourceCallsign.compare(0, prefix2.size(), prefix2) == 0)
                    {
                        sourceOK = true;
                    }
                }
            }

            // 4) Vérifier la validité de la destination (== "F4KMN-8")
            bool destOK = (destCallsign == "F4KMN-8");

            // 5) Déterminer si on a un code Q (QSA?, QSB?, QSC?, ...)
            QCode code = parseQCode(txt);

            // 6) Si on a détecté un code ET que la source et la destination correspondent,
            //    on construit la réponse spécifique.
            if (code != QCode::NONE && sourceOK && destOK)
            {
                // Ajustement de la taille du sourceCallsign sur 9 caractères EXACTS
                if (sourceCallsign.size() < 9) {
                    sourceCallsign.append(9 - sourceCallsign.size(), ' ');
                } else if (sourceCallsign.size() > 9) {
                    sourceCallsign.resize(9);
                }

                // Construire la réponse selon le code Q détecté
                std::ostringstream out;
                out << ":" << sourceCallsign << ":";

                switch (code)
                {
                case QCode::NONE:
                    // Rien à faire
                    break;

                case QCode::QSA:
                {
                    out << "QSA  RSSI=" << message.RSSI
                        << "dBm SNR=" << message.SNR
                        << "dB{" << idMessage;
                    break;
                }

                case QCode::QTR:
                {
                    // Récupère l'heure système (en temps local)
                    auto now = std::chrono::system_clock::now();
                    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

                    // Convertit en heure locale
                    std::tm local_tm = *std::localtime(&now_time);

                    // Formate l'heure en HH:MM:SS (par exemple)
                    char buffer[64];
                    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &local_tm);

                    out << "QTR " << buffer;
                    break;
                }

                case QCode::QSL:
                {
                    out << "QSL Confirmed, received.";
                    break;
                }

                default:
                    break;
                }

                // 7) Envoyer la réponse dans la file
                fileTX.ecrireDansLaFileIPC(out.str());
                ++idMessage;
            }

            // Pause de 1000 ms avant de relire
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception attrapée: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
