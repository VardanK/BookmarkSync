#include "sqlitedatabaseadapter.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

#define LOG_QUERY_ERROR(query) \
    qDebug() << __func__ << ":" << __LINE__ << " " \
    << query.executedQuery() << " [" << query.lastError().text() << "]";

#define LOG_DATABASE_ERROR(database) \
    qDebug() << __func__ << ":" << __LINE__ << " " \
    << database.lastError().text() ;

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
        LOG_DATABASE_ERROR(sqlDb);
    }
    else
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery query;
        query.prepare(QString(
                          "SELECT id, name FROM %1 "
                          "WHERE parent_id = :parentId").
                      arg(folderTableName));

        query.bindValue(":parentId", parentId);

        bool r = query.exec();

        if(r)
        {
            while(query.next())
            {
                qlonglong id = query.value(0).toLongLong();
                QString name = query.value(1).toString();

                folders.push_back(DatabaseUtils::FolderData(name, id, parentId));
            }
        }
        else
        {
            LOG_QUERY_ERROR(query);
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
        LOG_DATABASE_ERROR(sqlDb);
    }
    else
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery query;
        query.prepare(QString(
                          "SELECT id, url, tags, name FROM %1 "
                          "WHERE folder_id = :folderId").
                      arg(linksTableName));

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
            LOG_QUERY_ERROR(query);
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
        LOG_DATABASE_ERROR(sqlDb);
    }
    else
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery query;
        query.prepare(QString(
                          "INSERT INTO %1 (parent_id, name) "
                          "VALUES (:parentId, :folderName)").
                      arg(folderTableName));

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
            LOG_QUERY_ERROR(query);
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
        LOG_DATABASE_ERROR(sqlDb);
    }
    else
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery query;
        query.prepare(QString(
                          "UPDATE %1 "
                          "SET parent_id = :newParentId "
                          "WHERE id = :folderId").
                      arg(folderTableName));

        query.bindValue(":newParentId", newParentId);
        query.bindValue(":folderId", folderId);

        bool r = query.exec();

        if(r)
        {
            retVal = folderId;
        }
        else
        {
            LOG_QUERY_ERROR(query);
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
        LOG_DATABASE_ERROR(sqlDb);
    }
    else
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery query;
        query.prepare(QString(
                          "UPDATE %1 "
                          "SET name = :newName "
                          "WHERE id = :folderId").
                      arg(folderTableName));

        query.bindValue(":newName", newName);
        query.bindValue(":folderId", folderId);

        bool r = query.exec();

        if(r)
        {
            retVal = folderId;
        }
        else
        {
            LOG_QUERY_ERROR(query);
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
        LOG_DATABASE_ERROR(sqlDb);
    }
    else
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery query;
        query.prepare(QString(
                          "UPDATE %1 "
                          "SET name = :newName, "
                          "parent_id = :newParentId "
                          "WHERE id = :folderId").
                      arg(folderTableName));

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
            LOG_QUERY_ERROR(query);
        }
    }

    sqlDb.close();

    return retVal;
}

qlonglong SQLiteDatabaseAdapter::deleteFolder(qlonglong folderId)
{
    // Deleting the folder means that you need to recorsevely remove everything that
    // is under the folder (i.e. folders and links)
    QVector<DatabaseUtils::FolderData> subFolders = queryFolders(folderId);
    for(DatabaseUtils::FolderData folder : subFolders)
    {
        // Error reporting is done in deleteFolder function
        deleteFolder(folder.id);
    }

    QVector<DatabaseUtils::LinkData> links = queryLinks(folderId);
    for(DatabaseUtils::LinkData link : links)
    {
        // Error reporting is done in deleteLink function
        deleteLink(link.id);
    }

    qlonglong retVal = -1;

    if(!sqlDb.open())
    {
        LOG_DATABASE_ERROR(sqlDb);
    }
    else
    {
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery query;
        query.prepare(QString(
                        "DELETE FROM %1 "
                        "WHERE id = :folderId").
                      arg(folderTableName));

        query.bindValue(":folderId", folderId);

        bool r = query.exec();

        if(r)
        {
            retVal = folderId;
        }
        else
        {
            LOG_QUERY_ERROR(query);
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
        LOG_DATABASE_ERROR(sqlDb);
    }
    else
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery query;
        query.prepare(QString(
                         "INSERT INTO %1 (folder_id, url, tags, name) "
                         "VALUES (:folderId, :url, :tags, :name)").
                      arg(linksTableName));

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
            LOG_QUERY_ERROR(query);
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
        LOG_DATABASE_ERROR(sqlDb);
    }
    else
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery query;
        query.prepare(QString(
                        "UPDATE %1 "
                        "SET folder_id = :newParentId "
                        "WHERE id = :linkId").
                      arg(linksTableName));

        query.bindValue(":newParentId", newParentId);
        query.bindValue(":linkId", linkId);

        bool r = query.exec();

        if(r)
        {
            retVal = linkId;
        }
        else
        {
            LOG_QUERY_ERROR(query);
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
        LOG_DATABASE_ERROR(sqlDb);
    }
    else
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery query;
        query.prepare(QString(
                          "UPDATE %1 "
                          "SET name = :newName "
                          "WHERE id = :linkId").
                      arg(linksTableName));

        query.bindValue(":newName", newLinkName);
        query.bindValue(":linkId", linkId);

        bool r = query.exec();

        if(r)
        {
            retVal = linkId;
        }
        else
        {
            LOG_QUERY_ERROR(query);
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
        LOG_DATABASE_ERROR(sqlDb);
    }
    else
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery query;
        query.prepare(QString(
                          "UPDATE %1 "
                          "SET folder_id = :newFolderId, "
                          "url = :url, "
                          "tags = :tags, "
                          "name = :newName "
                          "WHERE id = :linkId").
                      arg(linksTableName));

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
            LOG_QUERY_ERROR(query);
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
        LOG_DATABASE_ERROR(sqlDb);
    }
    else
    {
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery query;
        query.prepare(QString(
                          "DELETE FROM %1 "
                          "WHERE id = :linkId").
                      arg(linksTableName));

        query.bindValue(":linkId", linkId);

        bool r = query.exec();

        if(r)
        {
            retVal = linkId;
        }
        else
        {
            LOG_QUERY_ERROR(query);
        }
    }

    sqlDb.close();

    return retVal;
}

void SQLiteDatabaseAdapter::initDatabase()
{
    if(!sqlDb.open())
    {
        LOG_DATABASE_ERROR(sqlDb);
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
        // Table does not exist, create it!
        // FOLDERS TABLE STRUCTURE
        // [id, parent_id, name]
        QSqlQuery queryCreate;
        isOk =  queryCreate.exec(QString(
                                 "CREATE TABLE %1 ("
                                 "id INTEGER PRIMARY KEY, "
                                 "parent_id INTEGER, "
                                 "name TEXT UNIQUE"
                                 ");").arg(folderTableName));

        if(!isOk)
        {
            LOG_QUERY_ERROR(queryCreate);
        }
        else
        {
            createDefaultFolders = true;
        }
    }
    else if(!isOk)
    {
        LOG_QUERY_ERROR(queryTables);
    }


    queryTables.prepare("SELECT name FROM sqlite_master WHERE type='table' AND "
                        "name = :tableName;");
    queryTables.bindValue(":tableName", linksTableName);

    isOk = queryTables.exec();

    if(isOk && !queryTables.next())
    {
        // Table does not exist, create it!
        // LINKS TABLE STRUCTURE
        // [id, folder_id, url, tags, name]
        QSqlQuery queryCreate;
        isOk =  queryCreate.exec(QString(
                                 "CREATE TABLE %1("
                                 "id INTEGER PRIMARY KEY, "
                                 "folder_id INTEGER, "
                                 "url TEXT UNIQUE, "
                                 "tags TEXT, "
                                 "name TEXT UNIQUE"
                                 ");").arg(linksTableName));

        if(!isOk)
        {
            LOG_QUERY_ERROR(queryCreate);
        }
    }
    else if(!isOk)
    {
        LOG_QUERY_ERROR(queryTables);
    }

    sqlDb.close();

    if(!isOk)
    {
        QMessageBox::critical(nullptr, "Database Error", "Cannot open/create database, please contact software owner to resolve the issue");
        exit(1);
    }

    if(createDefaultFolders)
    {
        // Create top level folder
        createFolder("Favorites", 0);
        createFolder("Sport", 0);
        createFolder("Work", 0);
        createFolder("Hobby", 0);
    }
}
