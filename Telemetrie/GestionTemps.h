
/*
 * File:   GestionTemps.h
 * Author: dbrochard
 *
 * Created on 27 mars 2024, 15:01
 */

#ifndef GestionTemps_H
#define GestionTemps_H

#include <iomanip>
#include <ctime>
#include <chrono>
#include <iostream>

/**
 * @struct Temps
 * @brief Structure pour stocker les composants d'une date et d'une heure.
 *
 * Cette structure permet de stocker les différents composants d'une date et d'une heure (année, mois, jour, heure, minute, seconde) sous forme de chaînes de caractères.
 */
struct Temps
{
    std::string annee{};    ///< L'année sous forme de chaîne
    std::string mois{};     ///< Le mois sous forme de chaîne
    std::string jour{};     ///< Le jour sous forme de chaîne
    std::string heure{};    ///< L'heure sous forme de chaîne
    std::string minute{};   ///< Les minutes sous forme de chaîne
    std::string seconde{};  ///< Les secondes sous forme de chaîne
};

/**
 * @class GestionTemps
 * @brief Classe permettant de gérer la date et l'heure actuelles.
 *
 * Cette classe permet de récupérer l'heure actuelle, de mettre à jour la date,
 * de formater la date et l'heure sous différents formats, et de générer des noms de fichiers basés sur la date.
 */
class GestionTemps
{

public:

    /**
     * @brief Constructeur de la classe GestionTemps.
     *
     * Initialise la classe et les membres nécessaires pour la gestion du temps.
     */
    GestionTemps(); // Constructeur

    /**
     * @brief Destructeur de la classe GestionTemps.
     *
     * Libère les ressources allouées par la classe, si nécessaire.
     */
    ~GestionTemps(); // Destructeur

    /**
     * @brief Obtient l'heure actuelle.
     *
     * Utilise la bibliothèque <ctime> pour obtenir l'heure actuelle sous forme d'un objet std::tm.
     * @return L'heure actuelle sous forme de structure std::tm.
     */
    std::tm obtenirHeureActuelle(); // Méthode pour obtenir l'heure actuelle

    /**
     * @brief Met à jour la date actuelle.
     *
     * Cette méthode met à jour les composants de la date actuelle (année, mois, jour, etc.) stockés dans la structure Temps.
     */
    void majDate();                 // Méthode pour mettre à jour la date

    /**
     * @brief Obtient la date et l'heure au format "aaaa-mm-jj hh:mm:ss".
     *
     * Cette méthode retourne la date et l'heure actuelles sous un format standardisé.
     * @return La date et l'heure sous forme de chaîne, formatée en "aaaa-mm-jj hh:mm:ss".
     */
    std::string getDateFormatee();  // Méthode pour obtenir la date et l'heure sous format aaaa-mm-jj hh:mm:ss

    /**
     * @brief Obtient la date et l'heure au format APRS (Mois/Jour Heure:Minute).
     *
     * Cette méthode retourne la date et l'heure au format utilisé par APRS (Automatic Packet Reporting System).
     * @return La date et l'heure au format "mois/jour heure:minute".
     */
    std::string getDateAprs();      // Méthode pour obtenir la date mois jour heure minute format aprs

    /**
     * @brief Génère un nom de fichier basé sur la date actuelle.
     *
     * Cette méthode génère un nom de fichier formaté avec la date et l'heure actuelles, utile pour les journaux ou les fichiers de données.
     * @return Le nom de fichier généré.
     */
    std::string generateFilename(); // Méthode pour générer un nom de fichier




private:
    Temps temps; ///< Variable pour stocker la date et l'heure actuelles

};

#endif /* GestionTemps_H */
