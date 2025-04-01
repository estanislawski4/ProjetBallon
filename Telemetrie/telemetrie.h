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


constexpr int ADDRESS_BME280 = 0x77;  // Adresse du capteur BME280

// Définition des valeurs minimales et maximales pour les mesures
constexpr double VAL_MIN_TEMPERATURE = -30.0;
constexpr double VAL_MAX_TEMPERATURE = 120.0;
constexpr double VAL_MIN_PRESSION = 0.0;
constexpr double VAL_MAX_PRESSION = 1100.0;
constexpr double VAL_MIN_HUMIDITE = 0.0;
constexpr double VAL_MAX_HUMIDITE = 100.0;
}

class VirguleDecimal : public std::numpunct<char> {
protected:
    // Surcharge de la fonction do_decimal_point() pour retourner une virgule
    virtual char do_decimal_point() const override {
        return ',';
    }
};

class Telemetrie
{
public:
    Telemetrie();
    string CreationTrameAPRS();
    void SauvegarderEnCsv();
    double getTemperature() const { return temperature; }
    double getPression() const { return pression; }
    double getHumidite() const { return humidite; }
private:

    void ObtenirTemperature(); // Fonction pour obtenir la température
    void ObtenirPression(); // Fonction pour obtenir la pression
    void ObtenirHumidite(); // Fonction pour obtenir l'humidité
    bool VerifierMesures(); // Focntion permet de vérifier les différentes mesures



    BME280 bme;
    GestionTemps gestionTemps;
    locale localeAvecVirgule;
    double temperature;
    double pression;
    double humidite;
    string fichierCSV;

};

#endif // TELEMETRIE_H
