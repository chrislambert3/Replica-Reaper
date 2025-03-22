// Copyright 2025 Replica Reaper
#include <QString>
#include <QListWidget>
#include <QDateTime>
#include <filesystem>
#include <iostream>
#include <string>
#include "FileInfo.hpp"
#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include "filemanager.hpp"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , manager(new FileManager()) {
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    // this links the button on the UI to the event function
    // "RunReaperBTN" is the name of the variable on the ui
    connect(ui->RunReaperBTN, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
}

MainWindow::~MainWindow() {
    delete ui;
    delete manager;
}

void MainWindow::onPushButtonClicked() {
    qDebug() << "Button clicked!";

    QString path = manager->PromptDirectory(this);
    QStringList filePaths = manager->ListFiles(path);

    // Set up the progress bar
    ui->progressBar->setValue(0);  // sets it to 0
    ui->progressBar->setMinimum(0);  // Min value
    ui->progressBar->setMaximum(filePaths.size());  // # of files to hash

    // Set a timer for hashing all files
    QElapsedTimer timer;
    timer.start();
    // Loop through each file and hash it (prints to console for now)
    for (int i = 0; i < filePaths.size(); ++i) {
        // setup for FileInfo class
        // QString file = filePaths[i];
        QByteArray hash = manager->HashFile(filePaths[i]);
        fs::path fPath = filePaths[i].toStdString();
        QDateTime currentDateTime = QDateTime::currentDateTime();
        FileInfo file(fPath, QString::fromStdString(fPath.extension().string()),
                      fs::file_size(fPath), hash, currentDateTime);
        // Push and sort FileInfo class into FileManager class
        manager->addFileToList(file);
        std::cout << *manager;  // DEBUG *************

        qDebug() << "File: " << filePaths[i] << "\nHash: " << hash;

        // Update progress bar
        ui->progressBar->setValue(i + 1);

        // Process events to keep UI responsive (for progress bar)
        QCoreApplication::processEvents();
    }
    // returns elapsed time in milliseconds ( /1000 for seconds)
    auto elapsedTime = timer.elapsed();
    QString message = "Took " + QString::number(elapsedTime / 1000.0, 'f') + " seconds";
    manager->ShowNotification("Hashing Complete", message);
    ShowDupesInUI(*manager);
}

// adds one file to qlistwidget
// duplicates will be located adjacently
// currently a stub function for testing
// currently just adds string including filepath and date
// Does not currently locate items adjacently
// NEED TO REFACTOR SO IT TAKES A FILEINFO
void MainWindow::ShowDupesInUI(const FileManager& f) {
    for (const auto& [fileType, MapSize] : f.AllFilesByTypeSize) {
        // out << "File Type: " << fileType.toStdString() << "\n";
        for (const auto& [fileSize, fileList] : MapSize) {
            // out << "  Size: " << fileSize << " bytes\n";
            for (const auto& file : fileList) {
                if (fileList.size() <= 1) {
                    std::cout << "am i in case 1********\n";
                    continue;   // throw exception?
                } else {
                    std::cout << "am i in case 2********\n";
                    QString out = QString::fromStdString(file.getFilePath().string());
                    // std::cout << file.getFilePathFuckQstring();
                    out.append("     ");
                    ui->listWidget->addItem(out);
                }
            }
        }
    }
/*


    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString ItemToAdd = FilePath;
    ItemToAdd.append("     ");
    ItemToAdd.append(currentDateTime.toString());
    ui->listWidget->addItem(ItemToAdd);
*/
    return;
}
