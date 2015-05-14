#include "addnewlink.h"
#include "ui_addnewlink.h"

#include <QApplication>
#include <QClipboard>
#include <QUrl>

AddNewLink::AddNewLink(QWidget *parent) :
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
}

AddNewLink::~AddNewLink()
{
    delete ui;
}
