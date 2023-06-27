#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql>
#include <QSqlDatabase>
#include <iostream>
class Database
{
public:
    Database();
    bool CreateConnection();
    bool CreateTables();
    bool DropTables();
    bool PushInfo(QString id, QString price, QString sale, QString rating, QString link);
};

#endif // DATABASE_H
