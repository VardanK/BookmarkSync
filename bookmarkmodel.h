#ifndef BOOKMARKMODEL_H
#define BOOKMARKMODEL_H

#include "sqlitedatabaseadapter.h"

#include <QList>
#include <QPair>
#include <QAbstractItemModel>

namespace ModelUtil
{
    enum EntryType {Folder = 0, Link, Invalid};
}

struct BookmarkItem
{
    BookmarkItem(const BookmarkItem& item);
    BookmarkItem(const QString &nm = QString("DEFAULT"),
                 const QString &ln = QString("no_url"),
                 const QString &tg = QString(""),
                 int i = -1,
                 ModelUtil::EntryType tp = ModelUtil::Invalid);

    int id;
    ModelUtil::EntryType type;
    QString url;
    QString name;
    QString tags;
};

class TreeItem
{

public:
    explicit TreeItem(const QString &name,
                      int id,
                      ModelUtil::EntryType type,
                      const QString &link,
                      const QString &tags,
                      TreeItem *parent = NULL);

    explicit TreeItem(const BookmarkItem &item,
                      TreeItem *parent = NULL);

    ~TreeItem();

    void addChild(TreeItem* child);

    void setName(const QString &newName);
    void setLink(const QString &newName);

    TreeItem *child(int num);
    int childCount() const;
    int columnCount() const;
    int row() const;

    bool insertChildren(int position, int count);
    bool insertChildren(int position, const BookmarkItem &item);
    bool removeChildren(int position, int count);

    bool insertChild(int position, const BookmarkItem &data);
    bool removeChild(int position);

    QString getName() const;
    QString getLink() const;
    ModelUtil::EntryType getType() const;
    int getId() const;

    TreeItem *getParent();


private:
    QList<TreeItem*> childList;
    BookmarkItem itemData;
    TreeItem *parent;
};

class BookmarkModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit BookmarkModel(QObject *parent = 0);
    ~BookmarkModel();

public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertRow(int row, const QModelIndex &index, const BookmarkItem &item);

    TreeItem* getItem(const QModelIndex &index) const;

private slots:
    void onFolderCreated(int folderId, int parentId);
    void onFolderMoved  (int folderId, int oldParent, int newParent);
    void onFolderUpdated(int folderId);
    void onFolderDeleted(int folderId, int parentId);

    void onLinkCreated(int linkId, int folderId);
    void onLinkMoved  (int linkId, int oldFolderId, int newFolderId);
    void onLinkUpdated(int linkId);
    void onLinkDeleted(int linkId, int folderId);

protected:
    void fillData();
    void addFolder(TreeItem* parent);


protected:
    SQLiteDatabaseAdapter database;
    TreeItem *rootItem;
};

#endif // BOOKMARKMODEL_H
