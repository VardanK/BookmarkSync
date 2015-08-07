#ifndef TESTDATABASE_H
#define TESTDATABASE_H

#include <QVector>

class SQLiteDatabaseAdapter;
namespace DatabaseUtils
{
    class FolderData;
}

class TestDatabase
{
public:
    TestDatabase(SQLiteDatabaseAdapter *adapter);
    ~TestDatabase();

public:
    void testBegin();

private:
    void testCreateFolder();
    void testRenameFolder();
    void testDeleteFolder();
    void testUpdateFolder();
    void testMoveFolder();

    void testCreateLink();
    void testRenameLink();
    void testDeleteLink();
    void testMoveLink();
    void testUpdateLink();

    void cleanup();

private:
    SQLiteDatabaseAdapter *dbAdapter;

    QVector<DatabaseUtils::FolderData> rootFolders;
};

#endif // TESTDATABASE_H
