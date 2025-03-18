#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void onPushButtonClicked();
    void addFileToList(const QString& FilePath, const QByteArray& hash);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    FileManager *manager;

};
#endif // MAINWINDOW_H
