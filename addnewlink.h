#ifndef ADDNEWLINK_H
#define ADDNEWLINK_H

#include "bookmarkmodel.h"

#include <QDialog>
#include <QModelIndex>

class BookmarkModel;
class QTimer;
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
    void onRequestTimer();

    void on_leLink_editingFinished();
    void on_leLink_textChanged(const QString &arg1);

private:
    void stopNetworkRequest();
    void updateFields();

    void startAnimation();
    void stopAnimation();

signals:
    void addUrl(const BookmarkItem &item, const QModelIndex &index);

private:
    // Ui and model
    Ui::AddNewLink *ui;
    BookmarkModel *model;

    // Netowrking
    QNetworkAccessManager *networkManager;
    QNetworkReply *lastRequest;
    QTimer *requestTimer;
};

#endif // ADDNEWLINK_H
