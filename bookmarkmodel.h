#ifndef BOOKMARKMODEL_H
#define BOOKMARKMODEL_H

#include <QList>
#include <QPair>
#include <QSqlDatabase>
#include <QAbstractItemModel>

class TreeItem
{
public:
    enum EntryType {Folder = 0, Link};

public:
    explicit TreeItem(const QString &name, int id, EntryType type, TreeItem *parent = NULL);
    ~TreeItem();

    void addChild(TreeItem* child);

    TreeItem *child(int num);
    int childCount() const;
    int columnCount() const;
    int row() const;

    QVariant getData(int column = 0) const;
    QVariant getType(int column = 0) const;
    int getId() const;

    TreeItem *getParent();


private:
    QList<TreeItem*> childList;

    QString name;
    EntryType type;
    TreeItem *parent;
    int id;
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

protected:
    QString queryBookmark(int folderId);

    QString getFolderName(int id) const;
    QString getBookmarkName(int id) const;

    void setFolderName(int id, QString value);
    void setBookmarkName(int id, QString value);

    void initDatabase();
    void fillData();
    void addFolder(TreeItem* parent);


protected:
    QSqlDatabase sqlDb;
    TreeItem *rootItem;
};

#endif // BOOKMARKMODEL_H
