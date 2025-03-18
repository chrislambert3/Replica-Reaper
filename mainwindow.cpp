#include <QString>
#include <QListWidget>
#include <QDateTime>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "filemanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,manager(new FileManager())
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    // this links the button on the UI to the event function
    // "RunReaperBTN" is the name of the variable on the ui
    connect(ui->RunReaperBTN, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete manager;
}

void MainWindow::onPushButtonClicked() {
    qDebug() << "Button clicked!";

    QString path = manager->PromptDirectory(this);
    QStringList filePaths = manager->ListFiles(path);

    // Set up the progress bar
    ui->progressBar->setValue(0); // sets it to 0
    ui->progressBar->setMinimum(0); // Min value
    ui->progressBar->setMaximum(filePaths.size()); // # of files to hash

    //Set a timer for hashing all files
    QElapsedTimer timer;
    timer.start();
    // Loop through each file and hash it (prints to console for now)
    for (int i = 0; i < filePaths.size(); ++i) {
        QString file = filePaths[i];
        QByteArray hash = manager->HashFile(file);
        qDebug() << "File: " << file << "\nHash: " << hash;

        // Update progress bar
        ui->progressBar->setValue(i + 1);

        // Process events to keep UI responsive (for progress bar)
        QCoreApplication::processEvents();
    }
    // returns elapsed time in milliseconds ( /1000 for seconds)
    auto elapsedTime = timer.elapsed();
    QString message = "Took " + QString::number(elapsedTime / 1000.0, 'f') + " seconds";
    manager->ShowNotification("Hashing Complete", message);
    addFileToList(path, QByteArray("")); // DEBUG
}

//adds one file to qlistwidget
//duplicates will be located adjacently
//currently a stub function for testing
//currently just adds string including filepath and date
//Does not currently locate items adjacently
//NEED TO REFACTOR SO IT TAKES A FILEINFO
void MainWindow::addFileToList(const QString& FilePath, const QByteArray& hash){
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString ItemToAdd = FilePath;
    ItemToAdd.append("     ");
    ItemToAdd.append(currentDateTime.toString());
    ui->listWidget->addItem(ItemToAdd);
    return;
}
