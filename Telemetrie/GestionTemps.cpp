/*
 * File:   GestionTemps.cpp
 * Author: dbrochard
 *
 * Created on 27 mars 2024, 15:01
 */

#include "GestionTemps.h"

/**
 * \brief Constructeur de GestionTemps
 * Initialise l'objet en mettant à jour la date.
 */
GestionTemps::GestionTemps()
{
    majDate();
}

/**
 * \brief Destructeur de GestionTemps
 */
GestionTemps::~GestionTemps()
{
}

/**
 * \brief Obtient l'heure actuelle en temps local
 * \return Structure std::tm représentant l'heure actuelle
 */
std::tm GestionTemps::obtenirHeureActuelle()
{
    std::chrono::time_point<std::chrono::system_clock> maintenant = std::chrono::system_clock::now();
    std::time_t tempsActuel = std::chrono::system_clock::to_time_t(maintenant);
    std::tm tmMaintenant = *localtime(&tempsActuel);
    return tmMaintenant;
}

/**
 * \brief Met à jour la structure de date et heure
 */
void GestionTemps::majDate()
{
    // Obtention du temps actuel
    auto tempsMaintenant = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::tm *tempsLocal = std::localtime(&tempsMaintenant);

    // Mise à jour des champs de la structure Temps
    temps.annee = std::to_string(tempsLocal->tm_year + 1900);
    if (tempsLocal->tm_year + 1900 < 1000)
        temps.annee = "0" + temps.annee;
    temps.mois = std::to_string(tempsLocal->tm_mon + 1);
    if (tempsLocal->tm_mon < 10)
        temps.mois = "0" + temps.mois;
    temps.jour = std::to_string(tempsLocal->tm_mday);
    if (tempsLocal->tm_mday < 10)
        temps.jour = "0" + temps.jour;
    temps.heure = std::to_string(tempsLocal->tm_hour);
    if (tempsLocal->tm_hour < 10)
        temps.heure = "0" + temps.heure;
    temps.minute = std::to_string(tempsLocal->tm_min);
    if (tempsLocal->tm_min < 10)
        temps.minute = "0" + temps.minute;
    temps.seconde = std::to_string(tempsLocal->tm_sec);
    if (tempsLocal->tm_sec < 10)
        temps.seconde = "0" + temps.seconde;


}

/**
 * \brief Retourne une date formatée pour APRS
 * \return Chaîne de caractères représentant la date au format MMJJHHmm
 */
std::string GestionTemps::getDateAprs(){
    majDate();
    return temps.mois + temps.jour + temps.heure + temps.minute;
}

/**
 * \brief Retourne la date formatée en format lisible
 * \return Chaîne de caractères représentant la date au format AAAA-MM-JJ HH:MM:SS CEST
 */
std::string GestionTemps::getDateFormatee()
{
    majDate();
    return temps.annee + "-" + temps.mois + "-" + temps.jour + " " + temps.heure + ":" + temps.minute + ":" + temps.seconde + " CEST";
}

/**
 * \brief Génère un nom de fichier basé sur la date et l'heure actuelles
 * \return Chaîne de caractères représentant le nom du fichier au format data_YYYY-MM-DD_HH-MM-SS.csv
 */
std::string GestionTemps::generateFilename(){
    majDate();
    return "data_" + temps.annee + "-" + temps.mois + "-" + temps.jour + "_" + temps.heure + "-" + temps.minute + "-" + temps.seconde + ".csv";
}
