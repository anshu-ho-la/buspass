#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QList>
#include <QPair>
#include "passwordutil.h"

class Database
{
public:
    static bool connect() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("buspass2.db");
        if (!db.open()) {
            return false;
        }

        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS user ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "username TEXT NOT NULL UNIQUE,"
                   "password TEXT NOT NULL CHECK(LENGTH(password) >= 6),"
                   "name TEXT NOT NULL,"
                   "email TEXT UNIQUE,"
                   "isAdmin BOOLEAN NOT NULL"
                   ")");

        query.exec("CREATE TABLE IF NOT EXISTS buses ("
                   "busID INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "busName TEXT NOT NULL,"
                   "route TEXT NOT NULL,"
                   "totalSeats INTEGER NOT NULL,"
                   "availableSeats INTEGER NOT NULL,"
                   "departureTime TEXT NOT NULL,"
                   "price REAL NOT NULL"
                   ")");

        query.exec("CREATE TABLE IF NOT EXISTS bookings ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "userid INTEGER NOT NULL,"
                   "busID INTEGER,"
                   "busName TEXT NOT NULL,"
                   "route TEXT NOT NULL,"
                   "departureTime TEXT NOT NULL,"
                   "seatsBooked INTEGER NOT NULL,"
                   "price REAL NOT NULL"
                   ")");

        query.exec("CREATE TABLE IF NOT EXISTS reviews ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "username TEXT NOT NULL,"
                   "review_text TEXT NOT NULL"
                   ")");

        QSqlQuery seedAdmin;
        seedAdmin.prepare("INSERT OR IGNORE INTO user (username, password, name, email, isAdmin) "
                          "VALUES ('admin', :password, 'admin1', 'admin@example.com', 1)");
        seedAdmin.bindValue(":password", PasswordUtil::hash("admin123"));
        seedAdmin.exec();

        // MIGRATION: hash any password that predates hashing being introduced.
        // Safe to run on every launch - already-hashed passwords are left alone.
        QSqlQuery userQuery;
        userQuery.exec("SELECT id, password FROM user");
        QList<QPair<int, QString>> toHash;
        while (userQuery.next()) {
            int id = userQuery.value(0).toInt();
            QString storedPassword = userQuery.value(1).toString();
            if (!PasswordUtil::looksHashed(storedPassword)) {
                toHash.append({id, storedPassword});
            }
        }
        for (const auto &pair : toHash) {
            QSqlQuery upgradeQuery;
            upgradeQuery.prepare("UPDATE user SET password = :hashed WHERE id = :id");
            upgradeQuery.bindValue(":hashed", PasswordUtil::hash(pair.second));
            upgradeQuery.bindValue(":id", pair.first);
            upgradeQuery.exec();
        }

        return true;
    }

private:
    static void addColumnIfMissing(QSqlQuery &query, const QString &table, const QString &column, const QString &type) {
        query.exec(QString("PRAGMA table_info(%1)").arg(table));
        bool exists = false;
        while (query.next()) {
            if (query.value(1).toString().compare(column, Qt::CaseInsensitive) == 0) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            query.exec(QString("ALTER TABLE %1 ADD COLUMN %2 %3").arg(table, column, type));
        }
    }
};

#endif // DATABASE_H
