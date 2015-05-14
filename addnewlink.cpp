#include "addnewlink.h"
#include "ui_addnewlink.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QClipboard>
#include <QUrl>

AddNewLink::AddNewLink(QAbstractItemModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewLink)
{
    ui->setupUi(this);

    QClipboard *clipboard = QApplication::clipboard();
    if(clipboard)
    {
        QString text = clipboard->text();
        // Check if it is correct URL
        QUrl url(text, QUrl::StrictMode);
        if(url.isValid())
        {
            ui->leLink->setText(text);
        }
    }

    ui->cbFolders->setModel(model);
}

AddNewLink::~AddNewLink()
{
    delete ui;
}
