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
        db.setDatabaseName("buspass.db");
        return db.open();

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
                   "userid INTEGER,"
                   "busName TEXT NOT NULL,"
                   "route TEXT NOT NULL,"
                   "bookedSeats INTEGER NOT NULL,"
                   "departureTime TEXT NOT NULL,"
                   "price REAL NOT NULL"
                   ")");
        query.exec("CREATE TABLE IF NOT EXISTS review ("
                   "userid INTEGER,"
                   "review TEXT"
                   ")");

      //query.exec("INSERT INTO user (username, password, name, email, isAdmin) VALUES ('admin', 'admin123', 'admin1', 'admin@example.com', 1)");
    }
};

#endif // DATABASE_H
