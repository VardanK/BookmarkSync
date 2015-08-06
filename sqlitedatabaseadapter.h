#ifndef SQLITEDATABASEADAPTER_H
#define SQLITEDATABASEADAPTER_H

#include <QSqlDatabase>
#include <QVector>

namespace DatabaseUtils
{
    struct FolderData
    {
        FolderData(const QString &name = QString("Unknown"),
                 qlonglong id = -1,
                 qlonglong parentId = -1
                 );

        QString name;
        qlonglong id;
        qlonglong parentId;
    };

    struct LinkData
    {
        LinkData(const QString &name = QString("Unknown"),
                 const QString &link = QString("no_url"),
                 const QString &tags = QString(""),
                 qlonglong id = -1,
                 qlonglong folderId = -1
                 );

        QString name;
        QString link;
        QString tags;
        qlonglong id;
        qlonglong folderId;
    };
}

class SQLiteDatabaseAdapter
{
public:
    SQLiteDatabaseAdapter();
    ~SQLiteDatabaseAdapter();

public:
    // Generic data query functions
    QVector<DatabaseUtils::FolderData> queryFolders(qlonglong parentId, const QString &filter = QString("")); // if parentId == will return all top level folders
    QVector<DatabaseUtils::LinkData> queryLinks(qlonglong folderId, const QString &filter = QString(""));

    // Folder manipulation interface
    qlonglong createFolder(const QString &name, qlonglong parentId); // -1 indicates error, othervise return created folder id
    qlonglong moveFolder  (qlonglong folderId, qlonglong newParentId);
    qlonglong renameFolder(qlonglong folderId, const QString &newName);
    qlonglong updateFolder(qlonglong folderId, const QString &newName, qlonglong newParentId);
    qlonglong deleteFolder(qlonglong folderId);

    // URL manipulation interface
    qlonglong createLink(const QString &name, const QString &url, const QString &tags, qlonglong folderId);
    qlonglong moveLink  (qlonglong linkId, qlonglong newParentId);
    qlonglong renameLink(qlonglong linkId, const QString &name);
    qlonglong updateLink(qlonglong linkId, const QString &name, const QString &url, const QString &tags, qlonglong folderId);
    qlonglong deleteLink(qlonglong linkId);

private:
    // Utility functions
    void initDatabase();

private:
    QSqlDatabase sqlDb;
    const QString databaseType;
    const QString databaseName;
    const QString folderTableName;
    const QString linksTableName;
    const int version;
};

#endif // SQLITEDATABASEADAPTER_H
