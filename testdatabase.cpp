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
    folders.clear();
    QVector<DatabaseUtils::FolderData> foldersOld = dbAdapter->queryFolders(0);

    qlonglong root1 = dbAdapter->createFolder("Test_Folder_Main_1", 0);
    qlonglong root2 = dbAdapter->createFolder("Test_Folder_Main_2", 0);
    qlonglong root3 = dbAdapter->createFolder("Test_Folder_Main_3", 0);

    COMPARE_NQ(root1, -1);
    COMPARE_NQ(root2, -1);
    COMPARE_NQ(root3, -1);

    qlonglong root1_sub1 = dbAdapter->createFolder("Test_SubFolder1_1", root1);
    qlonglong root1_sub2 = dbAdapter->createFolder("Test_SubFolder1_2", root1);
    qlonglong root1_sub3 = dbAdapter->createFolder("Test_SubFolder1_3", root1);


    COMPARE_NQ(root1_sub1, -1);
    COMPARE_NQ(root1_sub2, -1);
    COMPARE_NQ(root1_sub3, -1);

    qlonglong root2_sub1 = dbAdapter->createFolder("Test_SubFolder2_1", root2);
    qlonglong root2_sub2 = dbAdapter->createFolder("Test_SubFolder2_2", root2);
    qlonglong root2_sub3 = dbAdapter->createFolder("Test_SubFolder2_3", root2);

    COMPARE_NQ(root2_sub1, -1);
    COMPARE_NQ(root2_sub2, -1);
    COMPARE_NQ(root2_sub3, -1);

    qlonglong root3_sub1 = dbAdapter->createFolder("Test_SubFolder3_1", root3);
    qlonglong root3_sub2 = dbAdapter->createFolder("Test_SubFolder3_2", root3);
    qlonglong root3_sub3 = dbAdapter->createFolder("Test_SubFolder3_3", root3);

    COMPARE_NQ(root3_sub1, -1);
    COMPARE_NQ(root3_sub2, -1);
    COMPARE_NQ(root3_sub3, -1);

    QVector<DatabaseUtils::FolderData> foldersNew = dbAdapter->queryFolders(0);

    COMPARE_DIFF(foldersNew.size(), foldersOld.size(), 3)

    folders.push_back(root1);
    folders.push_back(root2);
    folders.push_back(root3);

    folders.push_back(root1_sub1);
    folders.push_back(root1_sub2);
    folders.push_back(root1_sub3);

    folders.push_back(root2_sub1);
    folders.push_back(root2_sub2);
    folders.push_back(root2_sub3);

    folders.push_back(root3_sub1);
    folders.push_back(root3_sub2);
    folders.push_back(root3_sub3);

    TEST_END
}

void TestDatabase::testRenameFolder()
{
    TEST_START

    qlonglong root1 = dbAdapter->renameFolder(folders.at(0), "Test_RenamedFirstRootFolder");
    COMPARE(root1, folders.at(0))


    qlonglong root2 = dbAdapter->renameFolder(folders.at(1), "Test_RenamedSecondRootFolder");
    COMPARE(root2, folders.at(1))

    qlonglong root1_sub1 = dbAdapter->renameFolder(folders.at(4), "Test_RenamedFirstRoot_SubFolder");
    COMPARE(root1_sub1, folders.at(4))

    TEST_END
}

void TestDatabase::testDeleteFolder()
{
    TEST_START

    QVector<DatabaseUtils::FolderData> foldersOld = dbAdapter->queryFolders(folders.at(2));
    qlonglong root3_sub3 = dbAdapter->deleteFolder(folders.at(11));
    QVector<DatabaseUtils::FolderData> foldersNew = dbAdapter->queryFolders(folders.at(2));

    COMPARE(root3_sub3, folders.at(11))
    COMPARE_DIFF(foldersOld.size(), foldersNew.size(), 1)

    TEST_END
}

void TestDatabase::testUpdateFolder()
{
    TEST_START

    qlonglong root1_sub3 = dbAdapter->updateFolder(folders.at(5), "Test_UpdateSub1_3Folder3", folders.at(2));
    COMPARE(root1_sub3, folders.at(5))

    qlonglong root2_sub3 = dbAdapter->updateFolder(folders.at(8), "Test_UpdateSub2_2Folder3", folders.at(1));
    COMPARE(root2_sub3, folders.at(8))

    TEST_END
}

void TestDatabase::testMoveFolder()
{
    TEST_START

    qlonglong root1_sub2 = dbAdapter->moveFolder(folders.at(4), folders.at(1));
    COMPARE(root1_sub2, folders.at(4))

    TEST_END
}

void TestDatabase::testCreateLink(){}
void TestDatabase::testRenameLink(){}
void TestDatabase::testDeleteLink(){}
void TestDatabase::testMoveLink(){}
void TestDatabase::testUpdateLink(){}

void TestDatabase::cleanup()
{
    // Just delete the top folders
    dbAdapter->deleteFolder(folders.at(0));
    dbAdapter->deleteFolder(folders.at(1));
    dbAdapter->deleteFolder(folders.at(2));
}
