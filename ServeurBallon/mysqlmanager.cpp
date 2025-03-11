#include "mysqlmanager.h"

#include <QDebug>

MySQLManager::MySQLManager(QObject *parent)
    : QObject(parent)
{
    // Configuration de la base de données selon votre config.inc PHP
    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName("195.221.60.234");
    m_db.setDatabaseName("Ballon2025");
    m_db.setUserName("ciel1_estanislawski");
    m_db.setPassword("P6wL9sF4");
}

MySQLManager::~MySQLManager() {
    closeConnection();
}

bool MySQLManager::openConnection() {
    if (!m_db.open()) {
        qDebug() << "Erreur de connexion à MySQL:" << m_db.lastError().text();
        return false;
    }
    return true;
}

void MySQLManager::closeConnection() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

QSqlQuery MySQLManager::executeQuery(const QString &queryStr) {
    QSqlQuery query(m_db);
    if (!query.exec(queryStr)) {
        qDebug() << "Erreur lors de l'exécution de la requête:" << query.lastError().text();
    }
    return query;
}

bool MySQLManager::machineExists(const QString &indicatif) {
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM machines WHERE indicatif = ?");
    query.addBindValue(indicatif);
    if (!query.exec()) {
        qDebug() << "Erreur machineExists:" << query.lastError().text();
        return false;
    }
    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool MySQLManager::insertMachine(const QString &indicatif, const QString &description) {
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO machines (indicatif, description) VALUES (?, ?)");
    query.addBindValue(indicatif);
    query.addBindValue(description);
    if (!query.exec()) {
        qDebug() << "Erreur insertMachine:" << query.lastError().text();
        return false;
    }
    return true;
}


bool MySQLManager::executeNonQuery(const QString &queryStr) {
    QSqlQuery query(m_db);
    if (!query.exec(queryStr)) {
        qDebug() << "Erreur lors de l'exécution de la requête non query:" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlDatabase MySQLManager::database() const {
    return m_db;
}
