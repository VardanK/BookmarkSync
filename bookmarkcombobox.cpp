#include "bookmarkcombobox.h"

#include <QMouseEvent>
#include <QHeaderView>
#include <QTreeView>
#include <QDebug>
#include <QEvent>

BookmarkComboBox::BookmarkComboBox(QWidget *parent) :
    QComboBox(parent)
{
    QTreeView *treeView = new QTreeView();
    treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView->setAnimated(true);
    treeView->header()->hide();

    treeView->viewport()->installEventFilter(this);

    setView(treeView);
}

bool BookmarkComboBox::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonRelease && object == view()->viewport())
    {
        // Combo box will try to close the view on mouse release
        // It is not good you user clicks on expand button, so look for that case and
        // filter the event by returning true
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QModelIndex index = view()->indexAt(mouseEvent->pos());
        if (!view()->visualRect(index).contains(mouseEvent->pos()))
            return true;

    }
    return false;
}

BookmarkComboBox::~BookmarkComboBox()
{
}

//void BookmarkComboBox::setModel(QAbstractItemModel *model)
//{
//    QComboBox::setModel(model);
//}

void BookmarkComboBox::setCurrentIndex(const QModelIndex &index)
{

    QModelIndex validIndex = index;
    if(!validIndex.isValid())
    {
        // If there is no index, try to get the first one from model
        validIndex = model()->index(0,0);
    }

    if(validIndex.isValid())
    {
        QTreeView *treeView = qobject_cast<QTreeView*>(view());
        if(treeView)
        {
            QModelIndex idx = validIndex;
            // Expand the tree to select the index
            do
            {
                treeView->setExpanded(idx, true);
                idx = idx.parent();
            }while(idx.isValid());


            treeView->selectionModel()->select(validIndex, QItemSelectionModel::Select);
            treeView->setCurrentIndex(validIndex);
            setCurrentText(validIndex.data().toString());
        }
    }
}

QModelIndex BookmarkComboBox::currentModelIndex() const
{
    return view()->currentIndex();
}
