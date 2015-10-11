#include "addnewlink.h"
#include "ui_addnewlink.h"

#include "bookmarkmodel.h"

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
    /*connect(networkManager, &QNetworkAccessManager::finished,
            this, &AddNewLink::onReplyFinished);
            */
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onReplyFinished(QNetworkReply*)));
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

    QTreeView *treeView = new QTreeView(ui->cbFolders);
    treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView->setAnimated(true);

    ui->cbFolders->setModel(model);
    ui->cbFolders->setView(treeView);

    ui->cbFolders->view()->viewport()->installEventFilter(this);

    if(parentIndex.isValid())
    {
        /*QModelIndex parentIdx = parentIndex;
        do
        {
            treeView->setExpanded(parentIdx, true);
            parentIdx = parentIdx.parent();
        }while(parentIdx.isValid());

        */
        //treeView->selectionModel()->select(parentIndex, QItemSelectionModel::Select);
        ui->cbFolders->setCurrentText(parentIndex.data().toString());

    }

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

void AddNewLink::on_leLink_editingFinished()
{
    stopNetworkRequest();

    if(ui->leName->text().isEmpty())
    {
        lastRequest = networkManager->get(QNetworkRequest(QUrl(ui->leLink->text())));
    }
}

void AddNewLink::onReplyFinished(QNetworkReply *reply)
{
    if(!ui->leName->text().isEmpty())
        return;

    QXmlStreamReader reader(reply->readAll());

    while (!reader.atEnd()) {
        reader.readNext();
        QStringRef tagName = reader.name();
        if(reader.isStartElement() && tagName.compare("title", Qt::CaseInsensitive) == 0)
        {
            ui->leName->setText(reader.readElementText());
            /*break*/;
        }
        else if(reader.isStartElement() && tagName.compare("meta", Qt::CaseInsensitive) == 0)
        {
            qDebug() << reader.readElementText();
        }

        if (reader.hasError()) {
            qWarning() << "Error reading reply...";
            break;
        }
    }
    /*
        QByteArray content= ;
        content.
        qDebug() << reply->header(((QNetworkRequest::KnownHeaders)i));
    */
}

void AddNewLink::stopNetworkRequest(){
    if(lastRequest != NULL && !lastRequest->isFinished())
    {
        lastRequest->abort();
        lastRequest->deleteLater();
        lastRequest = NULL;
    }
}
