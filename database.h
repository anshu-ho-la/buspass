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

      query.exec("INSERT INTO user (username, password, name, email, isAdmin) VALUES ('admin', 'admin123', 'admin1', 'admin@example.com', 1)");
    }
};

#endif // DATABASE_H
