#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>

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

        query.exec("INSERT OR IGNORE INTO user (username, password, name, email, isAdmin) VALUES ('admin', 'admin123', 'admin1', 'admin@example.com', 1);");


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
