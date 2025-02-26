#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "filemanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // this links the button on the UI to the event function
    // "pushButton" is the name of the variable on the ui
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onPushButtonClicked() {
    qDebug() << "Button clicked!";

    FileManager manager;
    QString path = manager.PromptDirectory(this);
    QStringList filePaths = manager.ListFiles(path);

    //Set a timer for hashing all files
    QElapsedTimer timer;
    timer.start();
    // Loop through each file and hash it (prints to console for now)
    for (int i = 0; i < filePaths.size(); ++i) {
        QString file = filePaths[i];
        QByteArray hash = manager.HashFile(file);
        qDebug() << "File: " << file << "\nHash: " << hash;
    }
    // returns elapsed time in milliseconds
    auto elapsedTime = timer.elapsed();
    qDebug() << "Hashing Complete, took" << elapsedTime/1000.0 << "seconds";

}
