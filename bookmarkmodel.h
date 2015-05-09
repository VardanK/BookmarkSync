#ifndef BOOKMARKMODEL_H
#define BOOKMARKMODEL_H

#include <QList>
#include <QPair>
#include <QSqlDatabase>
#include <QAbstractItemModel>

enum {FOLDER = 0, BOOKMARK} ENTRY_TYPE;

typedef QPair<int, int> Entry; // type and item id
typedef QList<Entry> EntryList; // row number and

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

protected:
    QString queryBookmark(int folderId);

    QString getFolderName(int id) const;
    QString getBookmarkName(int id) const;

    void setFolderName(int id, QString value);
    void setBookmarkName(int id, QString value);

    void initDatabase();
    void initDefaultData();

protected:
    QSqlDatabase sqlDb;
    EntryList bookmarks;
};

#endif // BOOKMARKMODEL_H
