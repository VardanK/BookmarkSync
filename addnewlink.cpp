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
#include <QTimer>
#include <QDebug>
#include <QMovie>
#include <QUrl>

AddNewLink::AddNewLink(BookmarkModel *md,
                       const QModelIndex& parentIndex,
                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewLink),
    model(md)
{
    ui->setupUi(this);

    // Network members initialization
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &AddNewLink::onReplyFinished);

    lastRequest = NULL;

    requestTimer = new QTimer(this);
    requestTimer->setSingleShot(true);
    connect(requestTimer,   &QTimer::timeout,
            this,           &AddNewLink::onRequestTimer);

    // Look for hyperlink in clipboard, if found set the link field value
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

    // Set the model and selected index
    ui->cbFolders->setModel(model);
    ui->cbFolders->setCurrentIndex(parentIndex);

    // Provide the feedback
    connect(this,SIGNAL(accepted()), this, SLOT(onAccepted()));

    // Prepare preloader
    QMovie *spinnerMovie = new QMovie(":/spinner.gif");
    ui->preloaderLabel->setMovie(spinnerMovie);
    spinnerMovie->start();
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
    QModelIndex index = ui->cbFolders->currentModelIndex();

    if(index.isValid())
    {
        // append to end
        model->insertRow(model->rowCount(index), index, item);
    }
}


void AddNewLink::onReplyFinished(QNetworkReply *reply)
{
    if(ui->leName->isKeyPressed())
        return;

    QXmlStreamReader reader(reply->readAll());

    while (!reader.atEnd()) {
        reader.readNext();

        QStringRef tagName = reader.name();
        bool isStart = reader.isStartElement();

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
    if(lastRequest != NULL)// && !lastRequest->isFinished())
    {
        lastRequest->abort();
        lastRequest->deleteLater();
        lastRequest = NULL;
    }
}

void AddNewLink::onRequestTimer()
{
    qDebug() << "Timer trigerred, updating the fields";
    updateFields();
}

void AddNewLink::updateFields()
{
    stopNetworkRequest();

    qDebug() << "Update Start";
    if(!ui->leName->isKeyPressed() && (!ui->leLink->text().isEmpty()))
    {
        qDebug() << "Update InProcess";
        lastRequest = networkManager->get(QNetworkRequest(QUrl(ui->leLink->text())));
    }
    qDebug() << "Update End";
}

void AddNewLink::startAnimation()
{
    ui->preloaderLabel->show();
}

void AddNewLink::stopAnimation()
{
    ui->preloaderLabel->hide();
}

void AddNewLink::on_leLink_editingFinished()
{
    updateFields();
}


void AddNewLink::on_leLink_textChanged(const QString &arg1)
{
    requestTimer->stop();
    requestTimer->start(700);
}
