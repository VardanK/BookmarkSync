#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QResource>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/new/main/res/ico/star.ico"));

    ui->webView->setHtml("<body><h1>Welcome</h1></body>");
}

MainWindow::~MainWindow()
{
    delete ui;
}
