#include "mysqlmanager.h"
#include <QDebug>

/**
 * @file MySQLManager.cpp
 * @brief Implémentation de la classe MySQLManager.
 *
 * Ce fichier contient l'implémentation des méthodes de la classe MySQLManager qui gère la connexion
 * à la base de données MySQL, l'exécution de requêtes SQL et les opérations spécifiques liées aux machines.
 */

/**
 * @brief Constructeur de la classe MySQLManager.
 *
 * Configure la connexion à la base de données MySQL avec les paramètres pré-définis.
 *
 * @param parent Pointeur vers l'objet parent (par défaut nullptr).
 */
MySQLManager::MySQLManager(QObject *parent)
    : QObject(parent)
{
    // Configuration de la connexion MySQL
    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName("195.221.60.234");
    m_db.setDatabaseName("Ballon2025");
    m_db.setUserName("ciel1_estanislawski");
    m_db.setPassword("P6wL9sF4");
}

/**
 * @brief Destructeur de la classe MySQLManager.
 *
 * Ferme la connexion à la base de données si elle est ouverte.
 */
MySQLManager::~MySQLManager()
{
    closeConnection();
}

/**
 * @brief Ouvre la connexion à la base de données.
 *
 * Tente d'ouvrir la connexion MySQL avec les paramètres configurés.
 *
 * @return bool @c true si la connexion a été établie avec succès, @c false sinon.
 */
bool MySQLManager::openConnection()
{
    bool success = true;
    if (!m_db.open()) {
        qDebug() << "Erreur de connexion à MySQL:" << m_db.lastError().text();
        success = false;
    }
    return success;
}

/**
 * @brief Ferme la connexion à la base de données.
 *
 * Ferme la connexion MySQL si elle est actuellement ouverte.
 */
void MySQLManager::closeConnection()
{
    if (m_db.isOpen())
        m_db.close();
}

/**
 * @brief Exécute une requête SQL de lecture.
 *
 * Crée un objet QSqlQuery associé à la connexion MySQL et exécute la requête fournie.
 * En cas d'erreur, un message est affiché dans la console de débogage.
 *
 * @param queryStr La requête SQL à exécuter.
 * @return QSqlQuery L'objet QSqlQuery contenant le résultat de la requête.
 */
QSqlQuery MySQLManager::executeQuery(const QString &queryStr)
{
    QSqlQuery query(m_db);
    if (!query.exec(queryStr)) {
        qDebug() << "Erreur lors de l'exécution de la requête:" << query.lastError().text();
    }
    return query;
}

/**
 * @brief Vérifie l'existence d'une machine dans la base de données.
 *
 * Exécute une requête préparée pour compter le nombre d'enregistrements correspondant
 * à l'indicatif fourni.
 *
 * @param indicatif L'indicatif de la machine à vérifier.
 * @return bool @c true si la machine existe (compte > 0), @c false sinon.
 */
bool MySQLManager::machineExists(const QString &indicatif)
{
    bool success = false;
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM machines WHERE indicatif = ?");
    query.addBindValue(indicatif);
    if (!query.exec()) {
        qDebug() << "Erreur machineExists:" << query.lastError().text();
        success = false;
    }
    if (query.next())
        success = query.value(0).toInt() > 0;
    return success;
}

/**
 * @brief Insère une nouvelle machine dans la base de données.
 *
 * Prépare et exécute une requête d'insertion pour ajouter une machine avec l'indicatif
 * et la description fournis.
 *
 * @param indicatif L'indicatif de la machine.
 * @param description La description de la machine.
 * @return bool @c true si l'insertion a réussi, @c false sinon.
 */
bool MySQLManager::insertMachine(const QString &indicatif, const QString &description)
{
    bool success = true;
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO machines (indicatif, description) VALUES (?, ?)");
    query.addBindValue(indicatif);
    query.addBindValue(description);
    if (!query.exec()) {
        qDebug() << "Erreur insertMachine:" << query.lastError().text();
        success = false;
    }
    return success;
}

/**
 * @brief Exécute une requête SQL de modification.
 *
 * Permet d'exécuter une requête SQL de type INSERT, UPDATE ou DELETE.
 *
 * @param queryStr La requête SQL à exécuter.
 * @return bool @c true si l'exécution s'est déroulée avec succès, @c false sinon.
 */
bool MySQLManager::executeNonQuery(const QString &queryStr)
{
    bool success = true;
    QSqlQuery query(m_db);
    if (!query.exec(queryStr)) {
        qDebug() << "Erreur lors de l'exécution de la requête non query:" << query.lastError().text();
        success = false;
    }
    return success;
}

/**
 * @brief Retourne l'objet QSqlDatabase utilisé par le gestionnaire.
 *
 * Permet d'accéder directement à l'objet QSqlDatabase pour des opérations supplémentaires.
 *
 * @return QSqlDatabase L'objet QSqlDatabase actuellement utilisé.
 */
QSqlDatabase MySQLManager::database() const
{
    return m_db;
}
