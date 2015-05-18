#include "bookmarkmodel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QIcon>


TreeItem::TreeItem(const QString &name, int id, ModelUtil::EntryType type, TreeItem *parent, const QString &link)
{
    itemData.name = name;
    itemData.id = id;
    itemData.type = type;
    itemData.url = link;

    this->parent = parent;

    if(this->parent)
        this->parent->addChild(this);
}

TreeItem::~TreeItem()
{
    qDeleteAll(childList);
}

void TreeItem::addChild(TreeItem* child)
{
    childList.append(child);
}

TreeItem* TreeItem::child(int num)
{
    return childList.value(num);
}

int TreeItem::childCount() const
{
    return childList.size();
}

int TreeItem::columnCount() const
{
    // Wh have onl name to show at thismoment, probably I'll add link later
    return 1;
}

int TreeItem::row() const
{
    if(parent)
    {
        return parent->childList.indexOf(const_cast<TreeItem*>(this));
    }

    return 0;
}

bool TreeItem::insertChildren(int position, int count)
{
    if(position < 0 || position > childList.size())
        return false;

    for(int r = 0; r < count; ++r)
    {
        TreeItem *item = new TreeItem("DEFAULT", -1, ModelUtil::Link, this);
        childList.insert(position, item);
    }

    return true;
}

bool TreeItem::removeChildren(int position, int count)
{
    if(position < 0 || (position + count) > childList.size())
        return false;

    for(int r = 0; r < count; ++r)
    {
        delete childList.takeAt(position);
    }

    return true;
}

int TreeItem::childNumber() const
{
    if(parent != NULL)
    {
        return parent->childList.indexOf(const_cast<TreeItem*>(this));
    }

    return 0;
}

int TreeItem::getId() const
{
    return itemData.id;
}

QVariant TreeItem::getType(int column) const
{
    Q_UNUSED(column);
    return itemData.type;
}

QVariant TreeItem::getData(int column) const
{
    Q_UNUSED(column);
    return itemData.name;
}

QVariant TreeItem::getLink(int column) const
{
    Q_UNUSED(column);
    return itemData.url;
}

void TreeItem::setName(const QString &newName)
{
    itemData.name = newName;
}

TreeItem* TreeItem::getParent()
{
    return parent;
}

BookmarkModel::BookmarkModel(QObject *parent) : QAbstractItemModel(parent)
{
    rootItem = NULL;
    initDatabase();
    fillData();
}

BookmarkModel::~BookmarkModel()
{
    sqlDb.close();
}

int BookmarkModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if(parent.column() > 0)
        return 0;

    if(!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int BookmarkModel::columnCount(const QModelIndex &parent) const
{
   if(parent.isValid())
       return static_cast<TreeItem*>(parent.internalPointer())->columnCount();

   return rootItem->columnCount();
}

QVariant BookmarkModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    switch(role)
    {
        case Qt::DisplayRole :
        {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            return item->getData(index.column());
        }
        case Qt::DecorationRole :
        {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

            if(item->getType(index.column()).toInt() == ModelUtil::Folder)
            {
                return QIcon(":/new/main/res/ico/folder.ico");
            }
            else if(item->getType(index.column()).toInt() == ModelUtil::Link)
            {
                return QIcon(":/new/main/res/ico/link.ico");
            }

            break;
        }
        case Qt::ToolTipRole :
        {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            QString tip;

            if(item->getType(index.column()).toInt() == ModelUtil::Link)
            {
                tip = item->getLink(index.column()).toString();
            }

            return tip;
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
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            if(item != NULL)
            {
                item->setName(value.toString());
            }

            return true;
            /*Entry e = bookmarks.at(index.row());
            if(e.first == FOLDER)
            {
                setFolderName(bookmarks.at(index.row()).second, value.toString());
            }
            else
            {
                setBookmarkName(bookmarks.at(index.row()).second, value.toString());
            }*/
            //break;
        }
    default:
        break;
    }

    return false;
}

QVariant BookmarkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return rootItem->getData(section);

    return QVariant();
}

Qt::ItemFlags BookmarkModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
            return 0;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    if(item->getType().toInt() == ModelUtil::Link)
        flags |= Qt::ItemIsEditable;

    return flags;
}

QModelIndex BookmarkModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if(!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if(childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex BookmarkModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(child.internalPointer());
    TreeItem *parentItem = childItem->getParent();

    if(parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

void BookmarkModel::initDatabase()
{
    sqlDb = QSqlDatabase::addDatabase("QSQLITE");
    sqlDb.setDatabaseName("bookmarks.sqlite");

    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
        exit(1);
    }

    // Check if tables exist
    QSqlQuery queryTables;
    queryTables.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='bookmark_folders';");
    if(!queryTables.next())
    {
        //Table does not exist
        QSqlQuery queryCreate;
        queryCreate.prepare("CREATE TABLE bookmark_folders ("
                         "id INTEGER PRIMARY KEY, "
                         "parent_id INTEGER, "
                         "name TEXT"
                         ");");

        if(queryCreate.exec())
        {
            // Fill with default data
            QSqlQuery queryAdd;
            bool r = queryAdd.exec("INSERT INTO bookmark_folders (id, parent_id, name) "
                             "VALUES (1, 0, 'Bookmarks'), "
                             "(2, 1, 'Favorites'), "
                             "(6, 2, 'Home'), "
                             "(7, 2, 'Work'), "
                             "(8, 2, 'Hobby'), "
                             "(3, 1, 'Chrome'), "
                             "(4, 1, 'Firefox'), "
                             "(5, 1, 'IE')");

            if(!r) qDebug() << queryAdd.lastError();
        }
        else
        {
            qDebug() << queryCreate.lastError();
        }
    }

    queryTables.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='bookmark_links';");
    if(!queryTables.next())
    {
        QSqlQuery queryCreate;
        queryCreate.prepare("CREATE TABLE bookmark_links ("
                         "id INTEGER PRIMARY KEY, "
                         "folder_id INTEGER, "
                         "url TEXT, "
                         "tags TEXT, "
                         "name TEXT"
                         ");");

        if(queryCreate.exec())
        {
            qDebug() << "Query successfull, links table created";
            QSqlQuery queryAdd;
            bool r = queryAdd.exec("INSERT INTO bookmark_links (folder_id, url, tags, name) "
                             "VALUES (2, 'http://microsoft.com', '', 'Microsoft'), "
                             "(2, 'http://valve.com', '', 'Valve'), "
                             "(3, 'http://google.com', '', 'Google'), "
                             "(2, 'http://mozilla.com', '', 'Mozilla'), "
                             "(6, 'http://mozilla.com', '', 'Mozilla'), "
                             "(7, 'http://mozilla.com', '', 'Mozilla'), "
                             "(6, 'http://mozilla.com', '', 'Mozilla'), "
                             "(8, 'http://mozilla.com', '', 'Mozilla'), "
                             "(6, 'http://mozilla.com', '', 'Mozilla'), "
                             "(6, 'http://mozilla.com', '', 'Mozilla'), "
                             "(1, 'http://gmail.com', '', 'GMail')");
            if(!r) qDebug() << queryAdd.lastError();
        }
        else
        {
            qDebug() << queryCreate.lastError();
        }
    }

    sqlDb.close();
}

void BookmarkModel::fillData()
{
    if(!sqlDb.open())
    {
        qDebug() << sqlDb.lastError();
        exit(1);
    }

    // Get the root element!
    QSqlQuery queryData;
    queryData.exec("SELECT name,id FROM bookmark_folders WHERE parent_id = '0'");

    if(queryData.next())
    {
        QString name = queryData.value(0).toString();
        int id = queryData.value(1).toInt();

        rootItem = new TreeItem(name, id, ModelUtil::Folder);
        addFolder(rootItem);
    }

    if(rootItem == NULL)
    {
        qDebug() << "DB Corrupted...";
        exit(2);
    }

    sqlDb.close();
}

void BookmarkModel::addFolder(TreeItem* parent)
{
    if(parent == NULL)
    {
        return;
    }

    QSqlQuery queryData;
    queryData.prepare("SELECT name,id FROM bookmark_folders WHERE parent_id = :folder_id");
    queryData.bindValue(":folder_id", parent->getId());
    queryData.exec();

    while(queryData.next())
    {
        QString name = queryData.value(0).toString();
        int id = queryData.value(1).toInt();

        addFolder(new TreeItem(name, id, ModelUtil::Folder, parent));
    }

    // Get the links
    QSqlQuery queryLinks;
    queryLinks.prepare("SELECT name,id,url FROM bookmark_links WHERE folder_id = :folder_id");
    queryLinks.bindValue(":folder_id", parent->getId());
    if(queryLinks.exec())
    {
        while(queryLinks.next())
        {
            QString name = queryLinks.value(0).toString();
            int id = queryLinks.value(1).toInt();
            QString url = queryLinks.value(2).toString();

            new TreeItem(name, id, ModelUtil::Link, parent, url);
        }

    }
    else
    {
        qDebug() << queryLinks.lastError();
    }


}
