#ifndef BOOKMARKCOMBOBOX_H
#define BOOKMARKCOMBOBOX_H

#include <QStyledItemDelegate>
#include <QComboBox>

class QEvent;

class BookmarkComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit BookmarkComboBox(QWidget *parent = 0);
    ~BookmarkComboBox();

public:
    void setCurrentIndex(const QModelIndex &index);

private:
    bool eventFilter(QObject* object, QEvent* event);
};

#endif // BOOKMARKCOMBOBOX_H
