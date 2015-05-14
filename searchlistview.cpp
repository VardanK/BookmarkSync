#include "searchlistview.h"
#include "ui_searchlistview.h"
#include "bookmarkmodel.h"

#include <QDebug>

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
}

SearchListView::~SearchListView()
{
    delete ui;
}

void SearchListView::onListItemSelected(const QModelIndex & index)
{
    if(index.isValid())
    {
        TreeItem *itm = static_cast<TreeItem*>(index.internalPointer());
        if(itm != NULL && itm->getType().toInt() == TreeItem::Link)
            emit urlSelected(itm->getLink().toString());
    }
}

void SearchListView::onFilterChanged(const QString &fliter)
{
    ;
}

