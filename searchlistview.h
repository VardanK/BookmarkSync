#ifndef SEARCHLISTVIEW_H
#define SEARCHLISTVIEW_H

#include <QWidget>

namespace Ui {
class SearchListView;
}

class SearchListView : public QWidget
{
    Q_OBJECT

public:
    explicit SearchListView(QWidget *parent = 0);
    ~SearchListView();

private:
    Ui::SearchListView *ui;
};

#endif // SEARCHLISTVIEW_H
