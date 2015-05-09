#include "searchlistview.h"
#include "ui_searchlistview.h"
#include "bookmarkmodel.h"

#include <QDebug>

SearchListView::SearchListView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchListView)
{
    ui->setupUi(this);


    BookmarkModel *model = new BookmarkModel();
    ui->bookmarks->setModel(model);
}

SearchListView::~SearchListView()
{
    delete ui;
}
