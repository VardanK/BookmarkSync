#include "addnewlink.h"
#include "ui_addnewlink.h"

#include "bookmarkmodel.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QMouseEvent>
#include <QClipboard>
#include <QTreeView>
#include <QDebug>
#include <QUrl>

AddNewLink::AddNewLink(BookmarkModel *model,
                       const QModelIndex& parentIndex,
                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewLink)
{
    ui->setupUi(this);

    QClipboard *clipboard = QApplication::clipboard();
    if(clipboard)
    {
        QString text = clipboard->text();
        if( text.startsWith("http:") ||
            text.startsWith("ftp:") ||
            text.startsWith("https:") ||
            text.startsWith("www."))
        {
            ui->leLink->setText(text);
        }
    }

    QTreeView *treeView = new QTreeView(ui->cbFolders);
    treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView->setAnimated(true);

    ui->cbFolders->setModel(model);
    ui->cbFolders->setView(treeView);

    ui->cbFolders->view()->viewport()->installEventFilter(this);

    if(parentIndex.isValid())
    {
        QModelIndex parentIdx = parentIndex;
        do
        {
            treeView->setExpanded(parentIdx, true);
            parentIdx = parentIdx.parent();
        }while(parentIdx.isValid());

        treeView->selectionModel()->select(parentIndex, QItemSelectionModel::Select);
        ui->cbFolders->setCurrentText(parentIndex.data().toString());

    }

}

bool AddNewLink::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonRelease && object == ui->cbFolders->view()->viewport())
    {
        // Combo box will try to close the view on mouse release
        // It is not good you user clicks on expand button, so look for that case and
        // filter the event by returning true
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QModelIndex index = ui->cbFolders->view()->indexAt(mouseEvent->pos());
        if (!ui->cbFolders->view()->visualRect(index).contains(mouseEvent->pos()))
            return true;

    }
    return false;
}

AddNewLink::~AddNewLink()
{
    delete ui;
}
