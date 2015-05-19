#include "searchlistview.h"
#include "ui_searchlistview.h"
#include "bookmarkmodel.h"

#include <QDebug>
#include <QMenu>

SearchListView::SearchListView(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::SearchListView)
{
    ui->setupUi(this);

    BookmarkModel *model = new BookmarkModel();
    ui->bookmarks->setModel(model);
    ui->bookmarks->header()->hide();

    connect(ui->bookmarks, SIGNAL(clicked(QModelIndex)),
            this, SLOT(onListItemSelected(QModelIndex)));

    connect(ui->filter, SIGNAL(textChanged(QString)),
            this, SLOT(onFilterChanged(QString)));

    connect(ui->bookmarks,
            SIGNAL(customContextMenuRequested(QPoint)),
            this,
            SLOT(onCustomContextMenu(QPoint)));
}

SearchListView::~SearchListView()
{
    delete ui;
}


BookmarkModel* SearchListView::model()
{
    return static_cast<BookmarkModel*>(ui->bookmarks->model());
}

void SearchListView::onListItemSelected(const QModelIndex & index)
{
    if(index.isValid())
    {
        TreeItem *itm = static_cast<TreeItem*>(index.internalPointer());
        if(itm != NULL && itm->getType() == ModelUtil::Link)
            emit urlSelected(itm->getLink());
    }
}

void SearchListView::onFilterChanged(const QString &fliter)
{
    ;
}

void SearchListView::onCustomContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->bookmarks->indexAt(pos);
    TreeItem *item = model()->getItem(index);
    if (index.isValid())
    {
        QAction *action = NULL;

        switch(item->getType())
        {
        case ModelUtil::Folder:
            {
                // Context menu for folders
                QMenu *contextMenu = new QMenu(this);
                contextMenu->addAction(ui->actionAddFolder);
                contextMenu->addAction(ui->actionRemoveFolder);
                contextMenu->addSeparator();
                contextMenu->addAction(ui->actionAddBookmark);
                contextMenu->addAction(ui->actionRemoveBookmark);
                action = contextMenu->exec(ui->bookmarks->mapToGlobal(pos));

                break;
            }
        case ModelUtil::Link:
            {
                // Context menu for links
                QMenu *contextMenu = new QMenu(this);
                contextMenu->addAction(ui->actionAddBookmark);
                contextMenu->addAction(ui->actionRemoveBookmark);
                action = contextMenu->exec(ui->bookmarks->mapToGlobal(pos));

                // New bookmark will be created in same folder!
                index = index.parent();

                break;
            }
        default:
            break;
        }

        if(action == ui->actionAddBookmark)
        {
            emit addBookmark(index);
        }
        else if(action == ui->actionRemoveBookmark)
        {
            emit removeBookmark(index);
        }
        else if(action == ui->actionAddFolder)
        {
            emit addFolder(index);
        }
        else if(action == ui->actionRemoveFolder)
        {
            emit removeFolder(index);
        }
    }
}

