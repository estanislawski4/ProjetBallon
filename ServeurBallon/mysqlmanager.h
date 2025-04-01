#ifndef MYSQLMANAGER_H
#define MYSQLMANAGER_H

/**
 * @file MySQLManager.h
 * @brief Déclaration de la classe MySQLManager.
 *
 * Ce fichier déclare la classe MySQLManager qui gère la connexion à une base de données MySQL,
 * permet d'exécuter des requêtes SQL et offre des méthodes spécifiques pour vérifier l'existence
 * d'une machine et l'insertion de nouvelles machines.
 */

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

/**
 * @brief Gestionnaire de connexion et d'opérations MySQL.
 *
 * La classe MySQLManager permet d'ouvrir et de fermer la connexion à une base de données MySQL,
 * d'exécuter des requêtes SQL (lecture et modification) et de réaliser des opérations spécifiques
 * liées à la gestion des machines dans la base de données.
 */
class MySQLManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructeur de la classe MySQLManager.
     * @param parent Pointeur vers l'objet parent (par défaut nullptr).
     */
    explicit MySQLManager(QObject *parent = nullptr);

    /**
     * @brief Destructeur de la classe MySQLManager.
     *
     * Ferme la connexion à la base de données si elle est ouverte.
     */
    ~MySQLManager();

    /**
     * @brief Ouvre la connexion à la base de données.
     *
     * Configure et ouvre la connexion MySQL en utilisant les paramètres pré-définis.
     *
     * @return bool @c true si la connexion est établie avec succès, @c false sinon.
     */
    bool openConnection();

    /**
     * @brief Ferme la connexion à la base de données.
     *
     * Ferme la connexion MySQL si celle-ci est actuellement ouverte.
     */
    void closeConnection();

    /**
     * @brief Exécute une requête SQL et retourne le résultat.
     *
     * Permet d'exécuter une requête de lecture (SELECT) et retourne un objet QSqlQuery contenant
     * les résultats de la requête.
     *
     * @param queryStr La chaîne de caractères contenant la requête SQL à exécuter.
     * @return QSqlQuery L'objet QSqlQuery contenant le résultat de la requête.
     */
    QSqlQuery executeQuery(const QString &queryStr);

    /**
     * @brief Exécute une requête SQL de modification.
     *
     * Permet d'exécuter une requête SQL de type INSERT, UPDATE ou DELETE.
     *
     * @param queryStr La chaîne de caractères contenant la requête SQL à exécuter.
     * @return bool @c true si l'exécution de la requête s'est déroulée avec succès, @c false sinon.
     */
    bool executeNonQuery(const QString &queryStr);

    /**
     * @brief Vérifie l'existence d'une machine dans la base de données.
     *
     * Exécute une requête pour déterminer si une machine avec l'indicatif spécifié existe déjà.
     *
     * @param indicatif L'indicatif de la machine à vérifier.
     * @return bool @c true si la machine existe, @c false sinon.
     */
    bool machineExists(const QString &indicatif);

    /**
     * @brief Insère une nouvelle machine dans la base de données.
     *
     * Insère une machine avec l'indicatif et la description fournis dans la table correspondante.
     *
     * @param indicatif L'indicatif de la machine.
     * @param description La description de la machine (optionnelle, valeur par défaut : chaîne vide).
     * @return bool @c true si l'insertion a réussi, @c false sinon.
     */
    bool insertMachine(const QString &indicatif, const QString &description = "");

    /**
     * @brief Retourne l'objet QSqlDatabase utilisé par le gestionnaire.
     *
     * Permet d'accéder directement à l'objet QSqlDatabase pour effectuer des opérations supplémentaires.
     *
     * @return QSqlDatabase L'objet QSqlDatabase actuellement utilisé.
     */
    QSqlDatabase database() const;

private:
    QSqlDatabase m_db; ///< Objet QSqlDatabase gérant la connexion à la base de données.
};

#endif // MYSQLMANAGER_H
