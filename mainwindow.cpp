// Copyright 2025 Replica Reaper
#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), manager(new FileManager()) {
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    this->setWindowTitle("Replica Reaper");
    this->setWindowIcon(QIcon(":/assets/assets/rr-logo.png"));
    manager->setMainWindow(this);
    // sets the background state
    this->backgroundCheck = false;

    // Tree Widget config:
    ui->treeWidget->setColumnCount(3);  // Single column for file names
    // Make columns resizable
    ui->treeWidget->header()->setSectionResizeMode(
        0, QHeaderView::Interactive);  // Filename
    ui->treeWidget->header()->setSectionResizeMode(
        1, QHeaderView::Interactive);  // File Path
    ui->treeWidget->header()->setSectionResizeMode(
        2, QHeaderView::ResizeToContents);  // Date Modified
    ui->treeWidget->header()->setMaximumSectionSize(300);
    ui->treeWidget->setHeaderLabels(
        {"Filename", "File Path", "Date Modified"});  // tree columns
    ui->treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->treeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->treeWidget->header()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents);  // File path length fits conted
    ui->treeWidget->header()->setSectionResizeMode(
        1, QHeaderView::ResizeToContents);  // Date Modified fits content
    // ui->treeWidget->setColumnWidth(0, 1000);
    // ui->treeWidget->setMinimumWidth(1000);
    // set specifation for the list tree
    // ui->treeWidget->setColumnHidden(1,true);
    // ui->treeWidget->setColumnWidth(2, 2); // Setting the second column to

    // Button Connections:
    // this links the button on the UI to the event function
    // "RunReaperBTN" is the name of the variable on the ui
    connect(ui->RunReaperBTN, &QPushButton::clicked, this,
            &MainWindow::onReaperButtonClicked);
    // Connect signal to handle parent-child checkbox logic
    connect(ui->treeWidget, &QTreeWidget::itemChanged, this,
            &MainWindow::onTreeItemChanged);
    // Connect the "DelSelBTN" to the onDelSelBTN_clicked function
    connect(ui->DelSelBTN, &QPushButton::clicked, this,
            &MainWindow::onDelSelBTN_clicked);
    // Connect the "DelAllBTN" to the onDelAllBTN_clicked function
    connect(ui->DelAllBTN, &QPushButton::clicked, this,
            &MainWindow::onDelAllBTN_clicked);
}

MainWindow::~MainWindow() {
    delete ui;
    delete manager;
    delete tutorial;
}

// Overloaded function that automatically gets called when user closes UI
void MainWindow::closeEvent(QCloseEvent *event) {
    // if the "Run in Background" button is checked,
    if (this->backgroundCheck) {
        // If checked, just close the window (keeps running in bacnground)
        event->accept();  // Accept the event, which will close the window
    } else {
        qApp->quit();  // Close the full application
        event->accept();
    }
}

void MainWindow::onReaperButtonClicked() {
    qDebug() << "Button clicked!";
    // clear the tree widget
    ui->treeWidget->clear();

    QString path = manager->PromptDirectory(this);
    if (path == QString()) {
        qDebug("Please select a directory");
        return;
    }
    // disable the button before reaping
    ui->RunReaperBTN->setEnabled(false);
    // set the status on the status bar
    ui->statusbar->showMessage("Scanning Files...");

    QStringList filePaths = manager->ListFiles(path);

    // Set up the progress bar
    ui->progressBar->setValue(0);                   // sets it to 0
    ui->progressBar->setMinimum(0);                 // Min value
    ui->progressBar->setMaximum(filePaths.size());  // # of files to hash

    // Set a timer for hashing all files
    QElapsedTimer timer;
    timer.start();
    qDebug() << "Total amount files to cover: " << filePaths.size();
    // Show the total amount in the status bar
    ui->statusbar->showMessage("Found " + QString::number(filePaths.size()) +
                             " files");
    // Loop through each file and hash it (prints to console for now)

    for (int i = 0; i < filePaths.size(); ++i) {
        // setup for FileInfo class
        fs::path fPath = filePaths[i].toStdString();
        FileInfo file(fPath, QString::fromStdString(fPath.extension().string()),
                    fs::file_size(fPath));
        // Push and sort FileInfo class into FileManager class
        manager->addFileToTypeSizeMap(file);  // passes in fileinfo
        // std::cout << *manager;  // DEBUG *************

        if (i % 50 == 0 || i == filePaths.size() - 1) {  // Update every 50 files
            ui->progressBar->setValue(i + 1);              // Update progress bar
            // Process events to keep UI responsive (for progress bar)
            QCoreApplication::processEvents();
        }
        // qDebug() << "File No: " << i;
    }

    // std::cout << *manager;  // DEBUG *************

    // returns elapsed time in milliseconds ( /1000 for seconds)
    auto elapsedTime = timer.elapsed();
    QString message =
        "Took " + QString::number(elapsedTime / 1000.0, 'f') + " seconds";
    manager->ShowNotification("Hashing Complete", message);
    ShowDupesInUI(*manager);
    // re-enable the button
    ui->RunReaperBTN->setEnabled(true);
    // clear the filemanager maps
    manager->ClearData();
}

// adds one file to qlistwidget
// duplicates will be located adjacently
// currently a stub function for testing
// currently just adds string including filepath and date
// Does not currently locate items adjacently
// NEED TO REFACTOR SO IT TAKES A FILEINFO
void MainWindow::ShowDupesInUI(const FileManager &f) {
    QString out;
    for (auto it = f.Dupes.begin(); it != f.Dupes.end(); ++it) {
        // Make a parent item for the list tree widget
        QTreeWidgetItem *parentHashItem = new QTreeWidgetItem(ui->treeWidget);
        parentHashItem->setText(0, it->second.front().getFileName());
        parentHashItem->setText(
            1, QString::fromStdString(
                it->second.front()
                    .getFilePath()
                    .string()));  // Next column value to go under FilePath
        parentHashItem->setText(2, it->second.front().getDate().toString());
        parentHashItem->setCheckState(0, Qt::Unchecked);  // Default unchecked
        // add the parent item to tree
        ui->treeWidget->addTopLevelItem(parentHashItem);

        for (auto &a : it->second) {
            out = QString::fromStdString(a.getFilePath().string());
            // Logic for adding to list Tree:
            // make a a child for the parent hash item
            QTreeWidgetItem *childItem = new QTreeWidgetItem(parentHashItem);
            childItem->setText(0, a.getFileName());  // set the filename
            childItem->setText(1, out);              // sets the filepath column
            childItem->setText(2, a.getDate().toString());
            childItem->setCheckState(0, Qt::Unchecked);
        }
    }
    ui->treeWidget->header()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents);  // Filename
    ui->treeWidget->header()->setSectionResizeMode(
        1, QHeaderView::Interactive);  // File Path
    ui->treeWidget->header()->setSectionResizeMode(
        2, QHeaderView::ResizeToContents);  // Date Modified
    ui->treeWidget->setColumnWidth(1, 200);
    ui->treeWidget->setColumnWidth(2, 150);
}

// Function to manage parent-child checkbox behavior
void MainWindow::onTreeItemChanged(QTreeWidgetItem *item) {
    if (!item) return;
    // Temporarily disconnect the signal to avoid recursion
    // (because changing child checkstates in this function triggers it again)
    disconnect(ui->treeWidget, &QTreeWidget::itemChanged, this,
                &MainWindow::onTreeItemChanged);

    // If a parent item check (group header) is toggled,
    if (item->parent() == nullptr) {
        // Get the check status of the parent
        Qt::CheckState newState = item->checkState(0);
        // set all the childs states to match the parents
        for (int i = 0; i < item->childCount(); ++i) {
            item->child(i)->setCheckState(0, newState);
        }
    } else {  // If a child item is toggled
        QTreeWidgetItem *parent = item->parent();
        bool allChecked = true, anyChecked = false;
        // compare all the child elements to see if some, all, or none are checked
        for (int i = 0; i < parent->childCount(); ++i) {
            Qt::CheckState state = parent->child(i)->checkState(0);
            if (state == Qt::Checked) anyChecked = true;
            if (state != Qt::Checked) allChecked = false;
        }

        // Update parent state based on children
        if (allChecked) {
            parent->setCheckState(0, Qt::Checked);
        } else if (anyChecked) {
            parent->setCheckState(0, Qt::PartiallyChecked);
        } else {  // none are checked
            parent->setCheckState(0, Qt::Unchecked);
        }
    }
    // Reconnect the signal after the change is done
    connect(ui->treeWidget, &QTreeWidget::itemChanged, this,
            &MainWindow::onTreeItemChanged);
}

void MainWindow::onDelSelBTN_clicked() {
    // prints selected to console for now
    // printCheckedItems();
    auto files = getCheckedItems();
    // dont show the dialoge if no files are selected
    if (files.empty()) {
        QMessageBox::information(this, "No Selection",
                             "Please select at least one file to delete.");
        return;
    }
    showDeleteConfirmation(files);
}
void MainWindow::onDelAllBTN_clicked() {
    // currently selects all buttons

    // Iterate through all top-level parent items
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *parentItem = ui->treeWidget->topLevelItem(i);

        // Set the parent item to checked
        parentItem->setCheckState(0, Qt::Checked);
        // Child items should automatically check due to onTreeItemChanged()
    }
}
list<pair<QString, QString>> MainWindow::getCheckedItems() {
    // pairs filename to filepath
    list<pair<QString, QString>> files;
    // iterate though the parent widgets
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *parentItem = ui->treeWidget->topLevelItem(i);

        // Skip if the parent is unchecked (so no childs are checked)
        if (parentItem->checkState(0) == Qt::Unchecked) {
            continue;
        }

        // Check the state of the parent item
        if (parentItem->checkState(0) == Qt::Checked) {
            qDebug() << "Checked Parent Group:" << parentItem->text(0);
        } else if (parentItem->checkState(0) == Qt::PartiallyChecked) {
            qDebug() << "Partially Checked Parent Group:" << parentItem->text(0);
        }

        // iterate through child items of the checked parent
        for (int j = 0; j < parentItem->childCount(); ++j) {
            QTreeWidgetItem *childItem = parentItem->child(j);

            // Skip if the child is unchecked (for partially checked cases)
            if (childItem->checkState(0) == Qt::Unchecked) {
                continue;
            } else {  // child is checked
                qDebug() << "Checked Item:" << childItem->text(0);
                auto filename = childItem->text(0);
                auto filepath = childItem->text(1);
                // add pairs to files
                std::pair<QString, QString> pair(filename, filepath);
                files.push_back(pair);
            }
        }
    }
    return files;
}

/* PythonAutoTestHelper() : runs the entire program. This
 * function is used solely for testing the run time of the
 * program. It is a slightly modified copy of OnReaperButtonClicked()
 *
 * Input: A directory path to be run time tested
 * output: Run time in milliseconds
 */
qint64 MainWindow::PythonAutoTestHelper(QString InputPath) {
    qDebug() << "Button clicked!";

    QString path = InputPath;
    QStringList filePaths = manager->ListFiles(path);

    QElapsedTimer timer;
    timer.start();
    qDebug() << "Total amount files to cover: " << filePaths.size();
    for (int i = 0; i < filePaths.size(); ++i) {
        fs::path fPath = filePaths[i].toStdString();
        FileInfo file(fPath, QString::fromStdString(fPath.extension().string()),
                  fs::file_size(fPath));
        manager->addFileToTypeSizeMap(file);
    }

    return timer.elapsed();
}
/* getDirectorySize() : Helper function for
 * finding the size of a directory
 *
 * Input: A directory path to be analyzed
 * output: size of file in bytes
 */
qint64 MainWindow::getDirectorySize(const QString &dirPath) {
    qint64 totalSize = 0;
    QDirIterator it(dirPath, QDir::Files | QDir::Hidden | QDir::NoSymLinks,
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        QFileInfo fileInfo(it.filePath());
        totalSize += fileInfo.size();
    }

    return totalSize;
}

void MainWindow::on_SettBTN_clicked() {
    Settings *settings = new Settings(this);
    settings->setState(this->backgroundCheck);
    settings->setModal(true);
    settings->exec();
}

void MainWindow::setBackgroundState(bool state) {
    this->backgroundCheck = state;
}

void MainWindow::showDeleteConfirmation(
        const list<pair<QString, QString>> &files) {
    QDialog dialog(this);
    dialog.setWindowTitle("Confirm Deletion");
    dialog.setModal(true);
    dialog.setMinimumSize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *label =
        new QLabel("Are you sure you want to delete the following files?");
    layout->addWidget(label);

    QListWidget *listWidget = new QListWidget();
    // Only add the first element of each pair (filename)
    for (const auto &filePair : files) {
        listWidget->addItem(filePair.first);
    }
    layout->addWidget(listWidget);

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No);
    layout->addWidget(buttonBox);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog,
                    &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog,
                   &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        // temperary "success flag"
        QMessageBox::information(this, "Success",
                             "Selected files deleted successfully.");
        // Delete files
        // Deleting File Logic:
        /*
        QStringList failedDeletions;

        for (const auto& filePair : files) {
            QString fullPath = filePair.second;
            QFile file(fullPath);
            if (!file.remove()) {
            failedDeletions.append(fullPath);
        }
    }

        if (!failedDeletions.isEmpty()) {
            QString errorMsg = "Failed to delete the following files:\n" +
        failedDeletions.join("\n"); QMessageBox::warning(this, "Deletion Failed",
        errorMsg); } else { QMessageBox::information(this, "Success", "Selected
        files deleted successfully.");
    }*/
    } else {
        // Canceled
    }
}

void MainWindow::on_HowUseBTN_clicked() {
    if (!tutorial) {
        tutorial = new Tutorial(this);
    }
    tutorial->show();
    tutorial->raise();           // Bring to front
    tutorial->activateWindow();  // Give focus
}
