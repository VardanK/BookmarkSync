#include "sqlitedatabaseadapter.h"
#include "testdatabase.h"

#include <assert.h>

#include <QDebug>

#define TEST_START \
    qDebug() << __func__ << " started";

#define COMPARE(actual, expected) \
    if(actual != expected) \
        qDebug() << __LINE__ << " comparision failed, expected " << expected \
        << ", got " << actual;

#define COMPARE_NQ(actual, expected) \
    if(actual == expected) \
        qDebug() << __LINE__ << " comparision failed, expected " << expected \
        << ", got " << actual;

#define COMPARE_DIFF(arg1, arg2, diff) \
    if(arg1 - arg2 != diff) \
        qDebug() << __LINE__ << " comparision failed, arg1(" << arg1  << ") - arg2(" \
        << arg2 << ") != "  << diff;

#define TEST_PRINT_FOLDERS(folders) \
    for(DatabaseUtils::FolderData folder : folders) \
    {qDebug() << "[F: " << folder.id << "] " << folder.name;}

#define TEST_END \
    qDebug() << __func__ << " finished";

TestDatabase::TestDatabase(SQLiteDatabaseAdapter *adapter)
{
    if(adapter == nullptr)
    {
        qDebug() << "Database adapter cannot be null pointer";
        exit(2);
    }

    this->dbAdapter = adapter;
}

TestDatabase::~TestDatabase()
{

}

void TestDatabase::testBegin()
{
    //cleanup();

    testCreateFolder();
    testRenameFolder();
    testDeleteFolder();
    testUpdateFolder();
    testMoveFolder();

    cleanup();
}

void TestDatabase::testCreateFolder()
{
    TEST_START

    rootFolders.clear();
    QVector<DatabaseUtils::FolderData> foldersOld = dbAdapter->queryFolders(0, "Test_");
    TEST_PRINT_FOLDERS(foldersOld)

    qlonglong root1 = dbAdapter->createFolder("Test_Folder_Main_1", 0);
    qlonglong root2 = dbAdapter->createFolder("Test_Folder_Main_2", 0);
    qlonglong root3 = dbAdapter->createFolder("Test_Folder_Main_3", 0);
    qlonglong root4 = dbAdapter->createFolder("Test_Folder_Main_4", 0);

    COMPARE_NQ(root1, -1);
    COMPARE_NQ(root2, -1);
    COMPARE_NQ(root3, -1);
    COMPARE_NQ(root4, -1);

    rootFolders = dbAdapter->queryFolders(0, "Test_");
    COMPARE_DIFF(rootFolders.size(), foldersOld.size(), 4)

    for(DatabaseUtils::FolderData folder : rootFolders)
    {
        // For each folder create subfolder
        int numSubFolders = 3;
        for(int i = 1; i <= numSubFolders; ++i)
        {
            qlonglong subFolder = dbAdapter->createFolder(QString("Test_SubFolder%1_%2").arg(folder.id).arg(i), folder.id);
            COMPARE_NQ(subFolder, -1);
        }

        QVector<DatabaseUtils::FolderData> createdSubFolder = dbAdapter->queryFolders(folder.id);
        COMPARE(createdSubFolder.size(), numSubFolders)
    }

    TEST_END
}

void TestDatabase::testRenameFolder()
{
    TEST_START

    qlonglong root1 = dbAdapter->renameFolder(rootFolders.at(0).id, "Test_RenamedFirstRootFolder");
    COMPARE(root1, rootFolders.at(0).id)

    QVector<DatabaseUtils::FolderData> subFolders = dbAdapter->queryFolders(rootFolders.at(2).id);

    qlonglong root1_sub1 = dbAdapter->renameFolder(subFolders.at(1).id, "Test_RenamedFirstRoot_SecondSubFolder");
    COMPARE(root1_sub1, subFolders.at(1).id)

    TEST_END
}

void TestDatabase::testDeleteFolder()
{
    TEST_START

    QVector<DatabaseUtils::FolderData> subFoldersBefore = dbAdapter->queryFolders(rootFolders.at(2).id);
    qlonglong deletedId = dbAdapter->deleteFolder(subFoldersBefore.at(0).id);
    QVector<DatabaseUtils::FolderData> subFoldersAfter = dbAdapter->queryFolders(rootFolders.at(2).id);

    COMPARE(deletedId, subFoldersBefore.at(0).id)
    COMPARE_DIFF(subFoldersBefore.size(), subFoldersAfter.size(), 1)

    TEST_END
}

void TestDatabase::testUpdateFolder()
{
    TEST_START

    QVector<DatabaseUtils::FolderData> rootFoldersOld = dbAdapter->queryFolders(0, "Test_");


    // Rename root folder 2 and make it subfolder for 1
    qlonglong root2 = dbAdapter->updateFolder(rootFolders.at(1).id, "Test_Root2", rootFolders.at(0).id);
    COMPARE(root2, rootFolders.at(1).id)

    rootFolders = dbAdapter->queryFolders(0, "Test_");
    COMPARE_DIFF(rootFoldersOld.size(), rootFolders.size(), 1)

    // Update subfolder without moving
    QVector<DatabaseUtils::FolderData> subFolders = dbAdapter->queryFolders(rootFolders.at(1).id);
    qlonglong updatedId = dbAdapter->updateFolder(subFolders.at(1).id, "Test_SubfolderUpdated", subFolders.at(1).parentId);

    COMPARE(updatedId, subFolders.at(1).id)

    TEST_END
}

void TestDatabase::testMoveFolder()
{
    TEST_START

    QVector<DatabaseUtils::FolderData> subFolders1Before = dbAdapter->queryFolders(rootFolders.at(0).id);
    QVector<DatabaseUtils::FolderData> subFolders2Before = dbAdapter->queryFolders(rootFolders.at(1).id);

    qlonglong movedId = dbAdapter->moveFolder(subFolders2Before.at(0).id, rootFolders.at(0).id);

    QVector<DatabaseUtils::FolderData> subFolders1After = dbAdapter->queryFolders(rootFolders.at(0).id);
    QVector<DatabaseUtils::FolderData> subFolders2After = dbAdapter->queryFolders(rootFolders.at(1).id);

    COMPARE(movedId, subFolders2Before.at(0).id)

    COMPARE_DIFF(subFolders1After.size(), subFolders1Before.size(), 1)
    COMPARE_DIFF(subFolders2Before.size(), subFolders2After.size(), 1)

    TEST_END
}

void TestDatabase::testCreateLink()
{}

void TestDatabase::testRenameLink()
{}

void TestDatabase::testDeleteLink()
{}

void TestDatabase::testMoveLink()
{}

void TestDatabase::testUpdateLink()
{}

void TestDatabase::cleanup()
{
    QVector<DatabaseUtils::FolderData> cleanupFolders = dbAdapter->queryFolders(0, "Test_");
    // Just delete the top folders
    for(DatabaseUtils::FolderData folder : cleanupFolders)
    {
        // For each folder create subfolder
        dbAdapter->deleteFolder(folder.id);
    }
}
