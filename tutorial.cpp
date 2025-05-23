// Copyright 2025 Replica Reaper
#include "tutorial.hpp"
#include "ui_tutorial.h"
#include "mainwindow.hpp"
/*Tutorial value constructor:
 *
 */
Tutorial::Tutorial(QWidget *parent) : QDialog(parent), ui(new Ui::Tutorial) {
    ui->setupUi(this);
    this->setWindowTitle("Tutorial");
    ui->textBrowser->setHtml(R"(
        <h2>How to Use Replica Reaper</h2>
        <p>1. Click <b>Run the Reaper</b> to scan for duplicate files.</p>
        <p>2. Review duplicates in the list below.</p>
        <p>3. Use <b>Delete selected</b> or <b>Delete all</b> to remove them.</p>
        <p>Need help? Visit our <a href='https://github.com/chrislambert3/Replica-Reaper'>Github</a>.</p>
    )");
    // allows for the link to open
    ui->textBrowser->setOpenExternalLinks(true);
    // Lambda to check if a path exists and has files within
    auto pathExists = [](const QString &path) -> bool {
        QDir dir(path);
        return dir.exists() && !dir.isEmpty();
    };

    connect(ui->tryDownBTN, &QPushButton::clicked, this, &Tutorial::ontryDownBTN_clicked);
    connect(ui->tryDocBTN,  &QPushButton::clicked, this, &Tutorial::ontryDocBTN_clicked);
    connect(ui->tryPicBTN,  &QPushButton::clicked, this, &Tutorial::ontryPicBTN_clicked);
    connect(ui->tryDeskBTN, &QPushButton::clicked, this, &Tutorial::ontryDeskBTN_clicked);

    // Collect the standard paths
    QString downloads =
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString documents =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString desktop =
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString pictures =
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

    // Disable buttons if the respective path doesn't exist
    if (!downloads.isEmpty() && !pathExists(downloads)) {
        ui->tryDownBTN->setEnabled(false);
    }
    if (!documents.isEmpty() && !pathExists(documents)) {
        ui->tryDocBTN->setEnabled(false);
    }
    if (!pictures.isEmpty() && !pathExists(pictures)) {
        ui->tryPicBTN->setEnabled(false);
    }
    if (!desktop.isEmpty() && !pathExists(desktop)) {
        ui->tryDeskBTN->setEnabled(false);
    }
}

void Tutorial::StartDemo(FileManager::FilesOrDownloads mode) {
    MainWindow *mainWindow = qobject_cast<MainWindow *>(parent());
    if (mainWindow) {
        mainWindow->setMode(mode);
        this->hide();
        mainWindow->onReaperButtonClicked();
    }
}

Tutorial::~Tutorial() { delete ui; }

void Tutorial::ontryDownBTN_clicked() {
    StartDemo(FileManager::Downloads);
}

void Tutorial::ontryDocBTN_clicked() {
    StartDemo(FileManager::Documents);
}

void Tutorial::ontryPicBTN_clicked() {
    StartDemo(FileManager::Pictures);
}

void Tutorial::ontryDeskBTN_clicked() {
    StartDemo(FileManager::Desktop);
}
