/**
 * @file telemetrie.cpp
 * @brief Implémentation de la classe Telemetrie
 * @version 1.2
 * @author Camille Mainfray
 * @date 25/03/2025
 *
 * @details Cette classe permet d'obtenir les mesures télémétriques du capteur BME280,
 *          de créer une trame APRS weather à partir des mesures et d'enregistrer
 *          ces mesures dans un fichier CSV.
 */

#include "telemetrie.h"  ///< Inclusion du fichier d'en-tête

/**
 * @brief Constructeur de la classe Telemetrie.
 *
 * @details Initialise l'objet BME280, configure la locale pour utiliser la virgule
 *          comme séparateur décimal et crée un fichier CSV pour stocker les mesures.
 *
 * @throws std::runtime_error si l'ouverture du fichier CSV échoue.
 */
Telemetrie::Telemetrie():
    bme(ADDRESS_BME280)  ///< Initialisation du capteur BME280
{
    // Création de la locale pour utiliser la virgule comme séparateur décimal
    localeAvecVirgule = std::locale(std::locale(), new VirguleDecimal);

    // Construction du nom du fichier CSV en y incluant un nom généré par la fonction gestionTemps.generateFilename()
    fichierCSV = "/home/cmainfray/" + gestionTemps.generateFilename();

    // Ouverture du fichier CSV en mode écriture (création d'un fichier si nécessaire)
    std::ofstream fichier(fichierCSV);
    if (!fichier.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir le fichier CSV.");
    }
    // Si le fichier est ouvert, on écrit les en-têtes de colonnes dans le fichier CSV
    fichier << "Date Time Time_Zone;Température;Pression;Humidité" << std::endl;
    fichier.close();
}

/**
 * @brief Génère une trame APRS weather.
 *
 * @details Cette fonction récupère les mesures, les valide et les formate
 *          selon la norme APRS pour la télémétrie météo.
 *
 * @return Une chaîne de caractères représentant la trame APRS.
 */
std::string Telemetrie::CreationTrameAPRS()
{
    // Appels des fonctions du BME pour obtenir les valeurs de température, humidité et pression
    temperature = bme.obtenirTemperatureEnF(); // Récupération des valeurs de température en °F
    humidite = bme.obtenirHumidite();
    pression = bme.obtenirPression();

    // Vérification que les mesures sont valides
    VerifierMesures();

    // Conversion des mesures en valeurs entières pour la trame APRS
    int t = static_cast<int>(temperature);  // Température (entière)
    int h = static_cast<int>(humidite + 0.5);  // Humidité (arrondie à l'entier le plus proche)
    int p = static_cast<int>(pression * 10);  // Pression (multipliée par 10 pour avoir la pression en dixièmes de mbar)

    std::ostringstream out;  // Création d'un flux de sortie (pour formater la trame APRS)
    out.imbue(localeAvecVirgule);  // Applique la locale avec la virgule comme séparateur décimal
    out << setfill('0') << fixed;  // Fixe la largeur de champ et utilise un format fixe
    out << "_" << gestionTemps.getDateAprs();  // Ajoute la date formatée pour la trame APRS
    out << "c...s...g...t" << std::setw(3) << t;  // Température (avec largeur de champ de 3)
    out << "h" << std::setw(2) << h;  // Humidité (avec largeur de champ de 2)
    out << "b" << std::setw(5) << p;  // Pression (avec largeur de champ de 5)
    out << showpos;  // Affiche le signe "+" devant les valeurs positives

    return out.str();  // Renvoie la trame APRS formatée sous forme de chaîne de caractères
}

/**
 * @brief Sauvegarde les mesures dans un fichier CSV.
 *
 * @details Enregistre les données télémétriques avec un format spécifique
 *          pour une exploitation ultérieure.
 *
 * @throws std::runtime_error si l'écriture dans le fichier échoue.
 */
void Telemetrie::SauvegarderEnCsv()
{
    // Appels des fonctions du BME pour obtenir les valeurs de température, humidité et pression
    temperature = bme.obtenirTemperatureEnC(); // Récupération des valeurs de température en °C
    humidite = bme.obtenirHumidite();
    pression = bme.obtenirPression();

    // Vérification que les mesures sont valides
    VerifierMesures();

    // Ouverture du fichier CSV en mode ajout (ajouter de nouvelles données sans écraser les anciennes)
    std::ofstream fichier(fichierCSV, std::ios_base::app);
    if (!fichier.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir le fichier CSV pour écriture.");
    }

    std::ostringstream out;  // Flux de sortie pour formater les valeurs
    out.imbue(localeAvecVirgule);  // Applique la locale avec la virgule comme séparateur décimal
    out << setfill('0') << fixed << setprecision(2) << "\"" << temperature << "\"";
    out << ";\"" << pression << "\"";
    out << ";\"" << humidite << "\"";
    out << showpos << std::endl;

    // Écriture de la date formatée et des mesures dans le fichier CSV
    fichier << gestionTemps.getDateFormatee();  // Date formatée
    fichier << ";";
    fichier << out.str();  // Valeurs formatées
    fichier.close();  // Fermeture du fichier
}

/**
 * @brief Vérifie la validité des mesures.
 *
 * @return true si toutes les mesures sont valides, false sinon.
 */
bool Telemetrie::VerifierMesures()
{
    bool valide = true;  // Variable pour indiquer si les mesures sont valides

    // Vérifie si la température est dans les limites acceptables
    if (temperature <= VAL_MIN_TEMPERATURE || temperature >= VAL_MAX_TEMPERATURE)
    {
        valide = false;
        temperature = NAN;
    }
    if (pression <= VAL_MIN_PRESSION || pression >= VAL_MAX_PRESSION)
    {
        valide = false;
        pression = NAN;
    }
    if (humidite <= VAL_MIN_HUMIDITE || humidite >= VAL_MAX_HUMIDITE)
    {
        valide = false;
        humidite = NAN;
    }
    return valide;
}
