#include "sqlitedatabaseadapter.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

namespace DatabaseUtils
{

    FolderData::FolderData(const QString &name, qlonglong id, qlonglong parentId)
    {
        this->name = name;
        this->id = id;
        this->parentId = parentId;
    }



    LinkData::LinkData(const QString &name, const QString &link,
             const QString &tags, qlonglong id, qlonglong folderId)
    {
        this->name = name;
        this->link = link;
        this->tags = tags;
        this->id = id;
        this->folderId = folderId;
    }

}

SQLiteDatabaseAdapter::SQLiteDatabaseAdapter() :
    databaseType("QSQLITE"),
    databaseName("bookmarks.sqlite"),
    folderTableName("bookmark_folders"),
    linksTableName("bookmark_links"),
    version(1)
{
    sqlDb = QSqlDatabase::addDatabase(databaseType);
    sqlDb.setDatabaseName(databaseName);
    initDatabase();
}

SQLiteDatabaseAdapter::~SQLiteDatabaseAdapter()
{
    if(sqlDb.isOpen())
    {
        qDebug() << "Please don't forget to close database after finishing the query";
        sqlDb.close();
    }
}

// Generic data query functions
QVector<DatabaseUtils::FolderData> SQLiteDatabaseAdapter::queryFolders(qlonglong parentId)
{
    QVector<DatabaseUtils::FolderData> folders;

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
    }
    else
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery query;
        query.prepare("SELECT id, parent_id, name FROM :tableName "
                      "WHERE parentId = :parentId");
        query.bindValue(":tableName", folderTableName);
        query.bindValue(":parentId", parentId);

        bool r = query.exec();

        if(r)
        {
            while(query.next())
            {
                qlonglong id = query.value(0).toLongLong();
                qlonglong parentId = query.value(1).toLongLong();
                QString name = query.value(2).toString();

                folders.push_back(DatabaseUtils::FolderData(name, id, parentId));
            }
        }
        else
        {
            qDebug() << query.lastError();
        }
    }

    sqlDb.close();

    return folders;
}

QVector<DatabaseUtils::LinkData> SQLiteDatabaseAdapter::queryLinks(qlonglong folderId)
{
    QVector<DatabaseUtils::LinkData> links;

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
    }
    else
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery query;
        query.prepare("SELECT id, url, tags, name FROM :tableName "
                      "WHERE folder_id = :folderId");
        query.bindValue(":tableName", linksTableName);
        query.bindValue(":folderId", folderId);

        bool r = query.exec();

        if(r)
        {
            while(query.next())
            {
                qlonglong id = query.value(0).toLongLong();
                QString  url = query.value(1).toString();
                QString tags = query.value(2).toString();
                QString name = query.value(3).toString();

                links.push_back(DatabaseUtils::LinkData(name, url, tags, id, folderId));
            }
        }
        else
        {
            qDebug() << query.lastError();
        }
    }

    sqlDb.close();

    return links;
}

// Folder manipulation interface
qlonglong SQLiteDatabaseAdapter::createFolder(const QString &name, qlonglong parentId)
{
    qlonglong retVal = -1;

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
    }
    else
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery query;
        query.prepare("INSERT INTO :tableName (parent_id, name) "
                         "VALUES (:parentId, :folderName)");
        query.bindValue(":tableName", folderTableName);
        query.bindValue(":parentId", parentId);
        query.bindValue(":folderName", name);

        bool r = query.exec();

        if(r)
        {
            QVariant lastInsert = query.lastInsertId();
            if(lastInsert.isValid())
                retVal = lastInsert.toLongLong();
        }
        else
        {
            qDebug() << query.lastError();
        }
    }

    sqlDb.close();

    return retVal;
}

qlonglong SQLiteDatabaseAdapter::moveFolder  (qlonglong folderId, qlonglong newParentId)
{
    qlonglong retVal = -1;

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
    }
    else
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery query;
        query.prepare("UPDATE :tableName "
                      "SET parent_id = :newParentId "
                      "WHERE id = :folderId");
        query.bindValue(":tableName", folderTableName);
        query.bindValue(":newParentId", newParentId);
        query.bindValue(":folderId", folderId);

        bool r = query.exec();

        if(r)
        {
            retVal = folderId;
        }
        else
        {
            qDebug() << query.lastError();
        }
    }

    sqlDb.close();

    return retVal;
}

qlonglong SQLiteDatabaseAdapter::renameFolder(qlonglong folderId, const QString &newName)
{
    qlonglong retVal = -1;

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
    }
    else
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery query;
        query.prepare("UPDATE :tableName "
                      "SET name = :newName "
                      "WHERE id = :folderId");
        query.bindValue(":tableName", folderTableName);
        query.bindValue(":newName", newName);
        query.bindValue(":folderId", folderId);

        bool r = query.exec();

        if(r)
        {
            retVal = folderId;
        }
        else
        {
            qDebug() << query.lastError();
        }
    }

    sqlDb.close();

    return retVal;
}

qlonglong SQLiteDatabaseAdapter::updateFolder(qlonglong folderId, const QString &newName,
                                              qlonglong newParentId)
{
    qlonglong retVal = -1;

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
    }
    else
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery query;
        query.prepare("UPDATE :tableName "
                      "SET name = :newName, "
                      "SET parent_id = :newParentId "
                      "WHERE id = :folderId");
        query.bindValue(":tableName", folderTableName);
        query.bindValue(":newName", newName);
        query.bindValue(":newParentId", newParentId);
        query.bindValue(":folderId", folderId);

        bool r = query.exec();

        if(r)
        {
            retVal = folderId;
        }
        else
        {
            qDebug() << query.lastError();
        }
    }

    sqlDb.close();

    return retVal;
}

qlonglong SQLiteDatabaseAdapter::deleteFolder(qlonglong folderId)
{
    qlonglong retVal = -1;

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
    }
    else
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery query;
        query.prepare("DELETE FROM :tableName "
                      "WHERE id = :folderId");
        query.bindValue(":tableName", folderTableName);
        query.bindValue(":folderId", folderId);

        bool r = query.exec();

        if(r)
        {
            retVal = folderId;
        }
        else
        {
            qDebug() << query.lastError();
        }
    }

    sqlDb.close();

    return retVal;
}

// URL manipulation interface
qlonglong SQLiteDatabaseAdapter::createLink(const QString &name, const QString &url,
                                            const QString &tags, qlonglong folderId)
{
    qlonglong retVal = -1;

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
    }
    else
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery query;
        query.prepare("INSERT INTO :tableName (folder_id, url, tags, name) "
                         "VALUES (:folderId, :url, :tags, :name)");
        query.bindValue(":tableName", linksTableName);
        query.bindValue(":folderId", folderId);
        query.bindValue(":url", url);
        query.bindValue(":tags", tags);
        query.bindValue(":name", name);

        bool r = query.exec();

        if(r)
        {
            QVariant lastInsert = query.lastInsertId();
            if(lastInsert.isValid())
                retVal = lastInsert.toLongLong();
        }
        else
        {
            qDebug() << query.lastError();
        }
    }

    sqlDb.close();

    return retVal;
}

qlonglong SQLiteDatabaseAdapter::moveLink  (qlonglong linkId, qlonglong newParentId)
{
    qlonglong retVal = -1;

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
    }
    else
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery query;
        query.prepare("UPDATE :tableName "
                      "SET folder_id = :newParentId "
                      "WHERE id = :linkId");
        query.bindValue(":tableName", linksTableName);
        query.bindValue(":newParentId", newParentId);
        query.bindValue(":linkId", linkId);

        bool r = query.exec();

        if(r)
        {
            retVal = linkId;
        }
        else
        {
            qDebug() << query.lastError();
        }
    }

    sqlDb.close();

    return retVal;
}

qlonglong SQLiteDatabaseAdapter::renameLink(qlonglong linkId, const QString &newLinkName)
{
    qlonglong retVal = -1;

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
    }
    else
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery query;
        query.prepare("UPDATE :tableName "
                      "SET name = :newName "
                      "WHERE id = :linkId");
        query.bindValue(":tableName", linksTableName);
        query.bindValue(":newName", newLinkName);
        query.bindValue(":linkId", linkId);

        bool r = query.exec();

        if(r)
        {
            retVal = linkId;
        }
        else
        {
            qDebug() << query.lastError();
        }
    }

    sqlDb.close();

    return retVal;
}

qlonglong SQLiteDatabaseAdapter::updateLink(qlonglong linkId, const QString &newName,
                                            const QString &url, const QString &tags,
                                            qlonglong folderId)
{
    qlonglong retVal = -1;

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
    }
    else
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery query;
        query.prepare("UPDATE :tableName "
                      "SET folder_id = :newFolderId, "
                      "url = :url, "
                      "tags = :tags, "
                      "name = :newName "
                      "WHERE id = :linkId");
        query.bindValue(":tableName", linksTableName);
        query.bindValue(":newFolderId", folderId);
        query.bindValue(":url", url);
        query.bindValue(":tags", tags);
        query.bindValue(":newName", newName);
        query.bindValue(":linkId", linkId);

        bool r = query.exec();

        if(r)
        {
            retVal = linkId;
        }
        else
        {
            qDebug() << query.lastError();
        }
    }

    sqlDb.close();

    return retVal;
}

qlonglong SQLiteDatabaseAdapter::deleteLink(qlonglong linkId)
{
    qlonglong retVal = -1;

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
    }
    else
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery query;
        query.prepare("DELETE FROM :tableName "
                      "WHERE id = :linkId");
        query.bindValue(":tableName", linksTableName);
        query.bindValue(":linkId", linkId);

        bool r = query.exec();

        if(r)
        {
            retVal = linkId;
        }
        else
        {
            qDebug() << query.lastError();
        }
    }

    sqlDb.close();

    return retVal;
}

void SQLiteDatabaseAdapter::initDatabase()
{
    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
        exit(1);
    }

    bool isOk = true;
    bool createDefaultFolders = false;

    // Check if tables exist
    QSqlQuery queryTables;
    queryTables.prepare("SELECT name FROM sqlite_master WHERE type='table' AND "
                        "name = :tableName;");
    queryTables.bindValue(":tableName", folderTableName);
    isOk = queryTables.exec();
    if(isOk && !queryTables.next())
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery queryCreate;
        queryCreate.prepare("CREATE TABLE :tableName ("
                         "id INTEGER PRIMARY KEY, "
                         "parent_id INTEGER, "
                         "name TEXT"
                         ");");
        queryCreate.bindValue(":tableName", folderTableName);

        isOk = queryCreate.exec();
        createDefaultFolders = true;
    }

    if(isOk)
    {
        queryTables.prepare("SELECT name FROM sqlite_master WHERE type='table' AND "
                            "name = :tableName;");
        queryTables.bindValue(":tableName", linksTableName);
        isOk = queryTables.exec();
    }

    if(isOk && !queryTables.next())
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery queryCreate;
        queryCreate.prepare("CREATE TABLE :tableName ("
                         "id INTEGER PRIMARY KEY, "
                         "folder_id INTEGER, "
                         "url TEXT, "
                         "tags TEXT, "
                         "name TEXT"
                         ");");

        queryCreate.bindValue(":tableName", linksTableName);

        isOk = queryCreate.exec();
    }

    if(!isOk)
    {
        qDebug() << sqlDb.lastError();
        exit(1);
    }

    sqlDb.close();

    if(createDefaultFolders)
    {
        // Create top level folder
        createFolder("Favorites", 0);
        createFolder("Sport", 0);
        createFolder("Work", 0);
        createFolder("Hobby", 0);
    }
}
