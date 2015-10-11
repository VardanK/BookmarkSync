#ifndef ADDNEWLINK_H
#define ADDNEWLINK_H

#include "bookmarkmodel.h"

#include <QDialog>
#include <QModelIndex>

class BookmarkModel;
class QNetworkReply;
class QNetworkAccessManager;

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
    void onReplyFinished(QNetworkReply*);
    void on_leLink_editingFinished();

private:
    void stopNetworkRequest();

signals:
    void addUrl(const BookmarkItem &item, const QModelIndex &index);

private:
    Ui::AddNewLink *ui;
    BookmarkModel *model;
    QNetworkAccessManager *networkManager;
    QNetworkReply *lastRequest;
};

#endif // ADDNEWLINK_H
