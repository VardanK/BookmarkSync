#ifndef TESTDATABASE_H
#define TESTDATABASE_H

#include <QVector>

class SQLiteDatabaseAdapter;

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

private:
    SQLiteDatabaseAdapter *dbAdapter;

    QVector<qlonglong> folders;
    QVector<qlonglong> links;
};

#endif // TESTDATABASE_H
