#ifndef SEARCHLISTVIEW_H
#define SEARCHLISTVIEW_H

#include <QFrame>
#include <QCompleter>

class QAbstractItemModel;
class BookmarkModel;

namespace Ui {
class SearchListView;
}

class SearchListView : public QFrame
{
    Q_OBJECT

public:
    explicit SearchListView(QWidget *parent = 0);
    ~SearchListView();

public:
    BookmarkModel* model();

signals:
    void urlSelected(const QString& url);
    void addBookmark(const QModelIndex& parent);
    void removeBookmark(const QModelIndex& item);

    void addFolder(const QModelIndex& parent);
    void removeFolder(const QModelIndex& item);

private slots:
    void onListItemSelected(const QModelIndex & index);
    void onFilterChanged(const QString &fliter);
    void onCustomContextMenu(const QPoint& pos);
private:
    Ui::SearchListView *ui;
    QCompleter *completer;
};

#endif // SEARCHLISTVIEW_H
