#include "bookmarkmodel.h"

#include <QSqlQuery>
#include <QDebug>

BookmarkModel::BookmarkModel(QObject *parent) : QAbstractItemModel(parent)
{
    initDatabase();
}

BookmarkModel::~BookmarkModel()
{
    sqlDb.close();
}

int BookmarkModel::rowCount(const QModelIndex &parent) const
{
    return bookmarks.count();
}

int BookmarkModel::columnCount(const QModelIndex &parent) const
{
    // We have only one column
    return 1;
}

QVariant BookmarkModel::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
        case Qt::DisplayRole :
        {
            Entry e = bookmarks.at(index.row());
            if(e.first == FOLDER)
            {
                return getFolderName(bookmarks.at(index.row()).second);
            }

            return getBookmarkName(bookmarks.at(index.row()).second);
        }

        default:
            break;
    }

    return QVariant();
}

bool BookmarkModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch(role)
    {
    case Qt::EditRole :
        {
            Entry e = bookmarks.at(index.row());
            if(e.first == FOLDER)
            {
                setFolderName(bookmarks.at(index.row()).second, value.toString());
            }
            else
            {
                setBookmarkName(bookmarks.at(index.row()).second, value.toString());
            }
            break;
        }
    default:
        break;
    }
}

QModelIndex BookmarkModel::index(int row, int column, const QModelIndex &parent) const
{
    ;
}

QModelIndex BookmarkModel::parent(const QModelIndex &child) const
{
    ;
}

QString BookmarkModel::getFolderName(int id) const
{
    return "";
}

QString BookmarkModel::getBookmarkName(int id) const
{
    return "";
}

void BookmarkModel::setFolderName(int id, QString value)
{
    ;
}

void BookmarkModel::setBookmarkName(int id, QString value)
{
    ;
}

void BookmarkModel::initDatabase()
{
    sqlDb = QSqlDatabase::addDatabase("QSQLITE");
    sqlDb.setDatabaseName("bookmarks.sqlite");

    if(sqlDb.open())
    {
        qDebug() << "DB opened!";
    }
    else
    {
        qDebug() << "Cannot connect to DB";
    }

    // Check if tables exist
    QSqlQuery queryTables;
    queryTables.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='bookmark_folders';");
    if(queryTables.next())
    {
        qDebug() << "Table exists...";
    }
    else
    {
        qDebug() << "Table does not exist...";
    }
}
