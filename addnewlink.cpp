#include "addnewlink.h"
#include "ui_addnewlink.h"

#include "bookmarkmodel.h"
#include "bookmarkcombobox.h"

#include <QNetworkAccessManager>
#include <QAbstractItemModel>
#include <QXmlStreamReader>
#include <QNetworkReply>
#include <QApplication>
#include <QMouseEvent>
#include <QClipboard>
#include <QTreeView>
#include <QDebug>
#include <QUrl>

AddNewLink::AddNewLink(BookmarkModel *md,
                       const QModelIndex& parentIndex,
                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewLink),
    model(md)
{
    ui->setupUi(this);

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &AddNewLink::onReplyFinished);

    lastRequest = NULL;

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

    ui->cbFolders->setModel(model);
    ui->cbFolders->setCurrentIndex(parentIndex);

    connect(this,SIGNAL(accepted()), this, SLOT(onAccepted()));
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

void AddNewLink::onAccepted()
{
    QString url = ui->leLink->text();
    QString name = ui->leName->text();
    QString tags = ui->leTags->text();

    BookmarkItem item(name, url, tags, -1, ModelUtil::Link);
    ui->cbFolders->itemData(ui->cbFolders->currentIndex());
    QModelIndex index = ui->cbFolders->view()->currentIndex();

    if(index.isValid())
        model->insertRow(model->rowCount(index), index, item);
}


void AddNewLink::onReplyFinished(QNetworkReply *reply)
{
    if(!ui->leName->text().isEmpty())
        return;

    QXmlStreamReader reader(reply->readAll());

    while (!reader.atEnd()) {
        reader.readNext();

        QStringRef tagName = reader.name();
        bool isStart = reader.isStartElement();

//        if(!reader.isCharacters())
//        {
//            qDebug() << tagName << " (" << reader.tokenType() << ") !";
//        }
//        else
//        {
//            qDebug() << tagName << "." " (" << reader.tokenType() << ") !";
//        }

        if(isStart && tagName.compare("title", Qt::CaseInsensitive) == 0)
        {
            ui->leName->setText(reader.readElementText(QXmlStreamReader::IncludeChildElements));
            break;
        }
        /*else if(isStart && (
                    tagName.compare("div", Qt::CaseInsensitive) == 0 ||
                    tagName.compare("p", Qt::CaseInsensitive) == 0
                ))
        {
            qDebug() << reader.readElementText();
        }*/


    }

    if (reader.hasError()) {
        qWarning() << reader.errorString();
    }
}

void AddNewLink::stopNetworkRequest(){
    if(lastRequest != NULL && !lastRequest->isFinished())
    {
        lastRequest->abort();
        lastRequest->deleteLater();
        lastRequest = NULL;
    }
}


void AddNewLink::on_leLink_editingFinished()
{
    stopNetworkRequest();

    if(ui->leName->text().isEmpty())
    {
        lastRequest = networkManager->get(QNetworkRequest(QUrl(ui->leLink->text())));
    }
}


void AddNewLink::on_leLink_textChanged(const QString &arg1)
{
    stopNetworkRequest();

    if(ui->leName->text().isEmpty())
    {
        lastRequest = networkManager->get(QNetworkRequest(QUrl(ui->leLink->text())));
    }
}
