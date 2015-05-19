#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QModelIndex>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onShowPreview(const QString& url);
    void onLoadStarted();
    void onLoadProgress(int progress);
    void onLoadFinished(bool st);
    void onShowAddNewURL(const QModelIndex &index = QModelIndex());

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
