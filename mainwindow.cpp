#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "addnewlink.h"
#include "searchlistview.h"

#include <QMessageBox>
#include <QResource>
#include <QUrl>

namespace {

    const int major_version = 0;
    const int minor_version = 1;

    const int longDelay = 15000; // 15s
    const int shortDelay = 3000; // 3s

    QString welcomePage = QObject::tr(

            "<!doctype html>"
            "<html>"
            "<head>"
                "<title>BookmarkSync  - version %1.%2</title>"
            "</head>"
            "<body style='background-color:%3;'>"
            "<h1 style='text-align: center;'>Thanks for using this software.</h1>"

            "<p>I hope you like it, have a nice day.</p>"
            "</body>"
            "</html>"
            ).arg(major_version).arg(minor_version);

    QString errorPage = QObject::tr(

            "<!doctype html>"
            "<html>"
            "<head>"
                "<title>BookmarkSync  - version %1.%2</title>"
            "</head>"
            "<body style='background-color:%3;'>"
            "<h1 style='text-align: center; color: %4;'>Sorry, something went wrong.</h1>"

            "<p style='color: %4;'>Cannot load page - %5.</p>"
            "</body>"
            "</html>"
            ).arg(major_version).arg(minor_version);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/new/main/res/ico/star.ico"));

    ui->webPreview->setHtml(welcomePage.arg("#B6B6B6"));

    connect(ui->navigator, SIGNAL(urlSelected(QString)),
            this, SLOT(onShowPreview(QString)));

    connect(ui->webPreview, SIGNAL(loadStarted()),
            this, SLOT(onLoadStarted()));

    connect(ui->webPreview, SIGNAL(loadProgress(int)),
            this, SLOT(onLoadProgress(int)));

    connect(ui->webPreview, SIGNAL(loadFinished(bool)),
            this, SLOT(onLoadFinished(bool)));

    connect(ui->actionAdd_Url, SIGNAL(triggered()),
            this, SLOT(onShowAddNewURL()));

    connect(ui->navigator, SIGNAL(addBookmark(QModelIndex)),
            this, SLOT(onShowAddNewURL(QModelIndex)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onShowPreview(const QString &address)
{
    QUrl url(address, QUrl::StrictMode);
    if(url.scheme().isEmpty())
    {
        url.setScheme("http");
    }

    ui->webPreview->setUrl(url);
    ui->webPreview->load(url);
}

void MainWindow::onLoadStarted()
{
    ui->statusBar->showMessage(tr("Loading page - %1").arg(ui->webPreview->url().toString()));
    setCursor(Qt::WaitCursor);
}

void MainWindow::onLoadProgress(int progress)
{
    ui->statusBar->showMessage(tr("Loading page - %1 %2%")
                                .arg(ui->webPreview->url().toString())
                                .arg(progress));
}

void MainWindow::onLoadFinished(bool st)
{
    if(!st)
    {
        ui->webPreview->setHtml(
                    errorPage.
                    arg("#3686BE"). // bg color
                    arg("#F7F9FB"). // txt color
                    arg(ui->webPreview->url().toString()));
        ui->statusBar->showMessage("Error loading data", longDelay);
    }
    else
    {
        ui->statusBar->showMessage("Page loaded", shortDelay);
    }

    setCursor(Qt::ArrowCursor);
}

void MainWindow::onShowAddNewURL(const QModelIndex &index)
{
    SearchListView *slv = ui->navigator;
    AddNewLink *dlg = new AddNewLink(slv->model(), index);
    dlg->exec();
}
