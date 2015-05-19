#ifndef ADDNEWLINK_H
#define ADDNEWLINK_H

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
    AddNewLink(BookmarkModel *model,
               const QModelIndex& parentIndex = QModelIndex(),
               QWidget *parent = 0);
    ~AddNewLink();

protected:
    bool eventFilter(QObject* object, QEvent* event);

private:
    Ui::AddNewLink *ui;
};

#endif // ADDNEWLINK_H
