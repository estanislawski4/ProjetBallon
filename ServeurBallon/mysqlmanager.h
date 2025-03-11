#ifndef MYSQLMANAGER_H
#define MYSQLMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class MySQLManager : public QObject {
    Q_OBJECT
public:
    explicit MySQLManager(QObject *parent = nullptr);
    ~MySQLManager();

    // Ouvre la connexion à la base de données.
    bool openConnection();

    // Ferme la connexion.
    void closeConnection();

    // Exécute une requête SQL et retourne le QSqlQuery.
    QSqlQuery executeQuery(const QString &queryStr);

    // Exécute une requête SQL (INSERT, UPDATE, DELETE) et retourne true en cas de succès.
    bool executeNonQuery(const QString &queryStr);

    bool machineExists(const QString &indicatif);

    bool insertMachine(const QString &indicatif, const QString &description = "");

    // Retourne l'objet QSqlDatabase (si besoin d'opérations supplémentaires).
    QSqlDatabase database() const;

private:
    QSqlDatabase m_db;
};

#endif // MYSQLMANAGER_H
