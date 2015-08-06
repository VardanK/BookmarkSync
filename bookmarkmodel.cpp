#include "bookmarkmodel.h"
#include "testdatabase.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QIcon>

BookmarkItem::BookmarkItem(const BookmarkItem& item):
    BookmarkItem(item.name, item.url, item.tags, item.id, item.type)
{
}

BookmarkItem::BookmarkItem(const QString &nm,
                           const QString &ln,
                           const QString &tg,
                           int i,
                           ModelUtil::EntryType tp)
{
    name = nm;
    url = ln;
    tags = tg;
    id = i;
    type = tp;
}

TreeItem::TreeItem(const QString &name,
                   int id,
                   ModelUtil::EntryType type,
                   const QString &link,
                   const QString &tags,
                   TreeItem *parent)
    : itemData(name, link, tags, id, type)
{
    this->parent = parent;
}

TreeItem::TreeItem(const BookmarkItem &item,
                   TreeItem *parent)
    : itemData(item)
{
    this->parent = parent;
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

    BookmarkItem defaultItem;

    for(int r = 0; r < count; ++r)
    {
        insertChild(position, defaultItem);
    }

    return true;
}


bool TreeItem::insertChildren(int position, const BookmarkItem &item)
{
    if(position < 0 || position > childList.size())
        return false;

    insertChild(position, item);

    return true;
}

bool TreeItem::removeChildren(int position, int count)
{
    if(position < 0 || (position + count) > childList.size())
        return false;

    for(int r = 0; r < count; ++r)
    {
        removeChild(position);
    }

    return true;
}

bool TreeItem::insertChild(int position, const BookmarkItem &data)
{
    if(position < 0 || position > childList.size())
        return false;

    TreeItem *item = new TreeItem(data, this);
    childList.insert(position, item);

    return true;
}

bool TreeItem::removeChild(int position)
{
    if(position < 0 || position >= childList.size())
        return false;

    delete childList.takeAt(position);
    return true;
}

int TreeItem::getId() const
{
    return itemData.id;
}

ModelUtil::EntryType TreeItem::getType() const
{
    return itemData.type;
}

QString TreeItem::getName() const
{
    return itemData.name;
}

QString TreeItem::getLink() const
{
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
    rootItem = nullptr;
    fillData();

    TestDatabase(&database).testBegin();
}

BookmarkModel::~BookmarkModel()
{
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
            return item->getName();
        }
        case Qt::DecorationRole :
        {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

            if(item->getType() == ModelUtil::Folder)
            {
                return QIcon(":/new/main/res/ico/folder.ico");
            }
            else if(item->getType() == ModelUtil::Link)
            {
                return QIcon(":/new/main/res/ico/link.ico");
            }

            break;
        }
        case Qt::ToolTipRole :
        {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            QString tip;

            if(item->getType() == ModelUtil::Link)
            {
                tip = item->getLink();
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
            if(item != nullptr && !value.toString().isEmpty())
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
    {
        switch(section)
        {
            case 0:
                return rootItem->getName();
            case 1:
                return rootItem->getLink();
            case 2:
                return rootItem->getType();
            default:
                break;
        }
    }

    return QVariant();
}

Qt::ItemFlags BookmarkModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
            return 0;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    if(item->getType() == ModelUtil::Link)
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

bool BookmarkModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row+count);
    bool ret = getItem(parent)->insertChildren(row, count);
    endInsertRows();
    return ret;
}

bool BookmarkModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row+count);
    bool ret = getItem(parent)->removeChildren(row, count);
    endRemoveRows();
    return ret;
}

bool BookmarkModel::insertRow(int row, const QModelIndex &parent, const BookmarkItem &item)
{
    beginInsertRows(parent, row, row+1);
    bool ret = getItem(parent)->insertChild(row, item);
    endInsertRows();
    return ret;
}

TreeItem* BookmarkModel::getItem(const QModelIndex &index) const
{
    if(index.isValid())
    {

        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if(item)
            return item;
    }

    return rootItem;
}

void BookmarkModel::fillData()
{
    if(rootItem == nullptr)
    {
        rootItem = new TreeItem("Root Folder", 0, ModelUtil::Folder, "Root Item", "");
    }

    addFolder(rootItem);
}

void BookmarkModel::addFolder(TreeItem* parent)
{
    if(parent == nullptr)
    {
        return;
    }

    // Query root folders
    QVector<DatabaseUtils::FolderData> folders = database.queryFolders(parent->getId());
    for(DatabaseUtils::FolderData folder : folders)
    {
        TreeItem *item = new TreeItem(folder.name, folder.id, ModelUtil::Folder, QString("")/*DUMMY LINK*/, QString("")/*DUMMY TAG*/, parent);
        parent->addChild(item);
        addFolder(item);
    }

    // Query root folders
    QVector<DatabaseUtils::LinkData> links = database.queryLinks(parent->getId());
    for(DatabaseUtils::LinkData link : links)
    {
        TreeItem *item = new TreeItem(link.name, link.id, ModelUtil::Link, link.link, link.tags, parent);
        parent->addChild(item);
    }
}
