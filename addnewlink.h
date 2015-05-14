#ifndef ADDNEWLINK_H
#define ADDNEWLINK_H

#include <QDialog>

namespace Ui {
class AddNewLink;
}

class AddNewLink : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewLink(QWidget *parent = 0);
    ~AddNewLink();

private:
    Ui::AddNewLink *ui;
};

#endif // ADDNEWLINK_H
