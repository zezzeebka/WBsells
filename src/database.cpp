#include "database.h"


Database::Database()
{

}

bool Database::CreateConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("postgres");
    db.setUserName("postgres");
    db.setPassword("123");
    if(!db.open())
    {
        std::cout << "db err";
        return false;
    }
    else
    {
        return true;
    }
}

bool Database::CreateTables()
{
    QSqlQuery query;
    query.exec("CREATE TABLE items ("
               "id VARCHAR(10) PRIMARY KEY, "
               "price VARCHAR(10) NOT NULL, "
               "sale VARCHAR(10), "
               "rating VARCHAR(10), "
               "link VARCHAR(100) NOT NULL)");
    return(1);
}

bool Database::DropTables()
{
    QSqlQuery query;
    QString deleteQuery = "DROP TABLE IF EXISTS items";

    if (query.exec(deleteQuery)) {
        qDebug() << "Таблица удалена успешно";
        return(1);
    } else {
        qDebug() << "Ошибка при удалении таблицы:" << query.lastError().text();
        return(0);
    }

}

bool Database::PushInfo(QString id, QString price, QString sale, QString rating, QString link)
{
    QSqlQuery query;
    QString insertQuery = "INSERT INTO items (id, price, sale,rating, link) VALUES (:id, :price, :sale, :rating, :link)";
    query.prepare(insertQuery);
    query.bindValue(":id", id);
    query.bindValue(":price", price);
    query.bindValue(":sale", sale);
    query.bindValue(":rating", rating);
    query.bindValue(":link", link);
    if (query.exec()) {
            qDebug() << "Данные успешно добавлены";
        } else {
            qDebug() << "Ошибка при добавлении данных:" << query.lastError().text();
        }

}
