#ifndef BOOKMARKMODEL_H
#define BOOKMARKMODEL_H

#include <QList>
#include <QPair>
#include <QSqlDatabase>
#include <QAbstractItemModel>

namespace ModelUtil
{
    enum EntryType {Folder = 0, Link};
}

struct BookmarkItem
{
    int id;
    ModelUtil::EntryType type;
    QString url;
    QString name;
};

class TreeItem
{

public:
    explicit TreeItem(const QString &name, int id, ModelUtil::EntryType type, TreeItem *parent = NULL, const QString &link = QString());
    ~TreeItem();

    void addChild(TreeItem* child);

    void setName(const QString &newName);
    void setLink(const QString &newName);

    TreeItem *child(int num);
    int childCount() const;
    int columnCount() const;
    int row() const;

    bool insertChildren(int position, int count);
    bool removeChildren(int position, int count);

    int childNumber() const;

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

protected:

    void initDatabase();
    void fillData();
    void addFolder(TreeItem* parent);


protected:
    QSqlDatabase sqlDb;
    TreeItem *rootItem;
};

#endif // BOOKMARKMODEL_H
