/**
 *@file telemetrie.h
 *@brief Declération de la classe Telemetrie
 *@version 1.2
 *@author Camille Mainfray
 *@date 25/03/2025
 */

#ifndef TELEMETRIE_H
#define TELEMETRIE_H

#include "BME280.h"
#include "GestionTemps.h"
#include <fstream>
#include <sstream>
namespace
{
    /**
     * @brief Adresse du capteur BME280
     */
    constexpr int ADDRESS_BME280 = 0x77;  // Adresse du capteur BME280

    /**
     * @brief Valeurs minimales et maximales pour les mesures
     */
// Définition des valeurs minimales et maximales pour les mesures
    constexpr double VAL_MIN_TEMPERATURE = -30.0;
    constexpr double VAL_MAX_TEMPERATURE = 120.0;
    constexpr double VAL_MIN_PRESSION = 0.0;
    constexpr double VAL_MAX_PRESSION = 1100.0;
    constexpr double VAL_MIN_HUMIDITE = 0.0;
    constexpr double VAL_MAX_HUMIDITE = 100.0;
}
 /**
 * @class VirguleDecimal
 * @brief Classe permettant de changer le séparateur décimal dans les flux.
 *
 * Cette classe surcharge la fonction `do_decimal_point()` pour retourner une virgule comme séparateur décimal.
 */
class VirguleDecimal : public std::numpunct<char> {
protected:
    // Surcharge de la fonction do_decimal_point() pour retourner une virgule
    virtual char do_decimal_point() const override {
        return ',';
    }
};

/**
 * @class Telemetrie
 * @brief Classe permettant de gérer la télémétrie d'un capteur BME280.
 *
 * Cette classe obtient les mesures de température, pression et humidité à partir du capteur BME280,
 * et les sauvegarde dans un fichier CSV au format approprié. Elle peut également générer une trame APRS.
 */
class Telemetrie
{
public:
    /**
     * @brief Constructeur de la classe Telemetrie.
     *
     * Initialise le capteur BME280 et les autres composants nécessaires à la collecte de données.
     */
    Telemetrie();

    /**
     * @brief Génère une trame APRS (Automatic Packet Reporting System).
     *
     * @return Une chaîne de caractères représentant la trame APRS formatée.
     */
    string CreationTrameAPRS();

    /**
     * @brief Sauvegarde les données de télémétrie dans un fichier CSV.
     *
     * Enregistre les mesures de température, pression et humidité dans un fichier CSV.
     */
    void SauvegarderEnCsv();

    /**
     * @brief Obtient la température mesurée par le capteur.
     *
     * @return La température en degrés Celsius.
     */
    double getTemperature() const { return temperature; }

    /**
     * @brief Obtient la pression mesurée par le capteur.
     *
     * @return La pression en mbar.
     */
    double getPression() const { return pression; }

    /**
     * @brief Obtient l'humidité mesurée par le capteur.
     *
     * @return L'humidité relative en pourcentage (%).
     */
    double getHumidite() const { return humidite; }
private:

    /**
     * @brief Vérifie la validité des mesures obtenues.
     *
     * Cette fonction permet de s'assurer que les valeurs mesurées par le capteur sont dans des limites acceptables.
     *
     * @return True si les mesures sont valides, false sinon.
     */
    bool VerifierMesures(); // Focntion permet de vérifier les différentes mesures

    /**
     * @brief Instance de la classe GestionTemps pour gérer les aspects temporels.
     */
    BME280 bme;

    /**
     * @brief Instance de la classe GestionTemps pour gérer les aspects temporels.
     */
    GestionTemps gestionTemps;

    /**
     * @brief Locale personnalisée pour utiliser la virgule comme séparateur décimal.
     */
    locale localeAvecVirgule;

    /**
     * @brief Température mesurée.
     */
    double temperature;

    /**
     * @brief Pression mesurée.
     */
    double pression;

    /**
     * @brief Humidité mesurée.
     */
    double humidite;

    /**
     * @brief Nom du fichier CSV où les données sont sauvegardées.
     */
    string fichierCSV;

};

#endif // TELEMETRIE_H
