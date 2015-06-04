#ifndef ADDNEWLINK_H
#define ADDNEWLINK_H

#include "bookmarkmodel.h"

#include <QDialog>
#include <QModelIndex>

class BookmarkModel;

namespace Ui {
class AddNewLink;
}

class AddNewLink : public QDialog
{
    Q_OBJECT

public:
    AddNewLink(BookmarkModel *md,
               const QModelIndex& parentIndex = QModelIndex(),
               QWidget *parent = 0);
    ~AddNewLink();

protected:
    bool eventFilter(QObject* object, QEvent* event);

private slots:
    void onAccepted();

signals:
    void addUrl(const BookmarkItem &item, const QModelIndex &index);

private:
    Ui::AddNewLink *ui;
    BookmarkModel *model;
};

#endif // ADDNEWLINK_H
