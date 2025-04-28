// Copyright 2025 Replica Reaper
#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
        manager(new FileManager()), trayIcon(new QSystemTrayIcon()), Qui(nullptr) {
    ui->setupUi(this);
    // hide the cancel button initially
    ui->CancelBTN->setVisible(false);
    // ui->CancelBTN->setObjectName("CancelBTN");

    // Set up UI styling file
    QFile styleFile(":/assets/assets/styles.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = styleFile.readAll();
        this->setStyleSheet(style);  // Applies only to this window and its widgets
    }
    ui->progressBar->setValue(0);
    this->setWindowTitle("Replica Reaper");
    // :/assets/assets... will resolve universally
    this->setWindowIcon(QIcon(":/assets/assets/rr-logo.png"));
    setQMainWindow(this);

    // Tree Widget config:
    ui->treeWidget->setColumnCount(3);  // Single column for file names
    ui->treeWidget->setHeaderLabels(
        {"Filename", "File Path", "Date Modified"});  // tree columns
    ui->treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->treeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

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
    // Connect the "CancelBTN" to the onCancelBTN_clicked function
    connect(ui->CancelBTN, &QPushButton::clicked, this,
            &MainWindow::onCancelBTN_clicked);
    // Connect the "SettBTN" to the onSettBTN_clicked function
    connect(ui->SettBTN, &QPushButton::clicked, this,
            &MainWindow::onSettBTN_clicked);
    // Connect the "HowUseBTN" to the onHowUseBTN_clicked function
    connect(ui->HowUseBTN, &QPushButton::clicked, this,
            &MainWindow::onHowUseBTN_clicked);

    // Tray icon shit start

    // can also set our custon icon like this:

    // Creates a tray icon when Program is hidden
    // Related to background process feature
    // trayIcon->setIcon(QIcon(":/icon.png")); // Set an icon (optional, ensure
    this->trayIcon->setIcon(QIcon(":/assets/assets/rr-logo.png"));
    quitAction = new QAction("Exit Replica Reaper", this);
    // map the action to close the full application, (qApp is a universal pointer
    // to the running application)
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    // add a trayicon menu with the quit button/action
    QMenu* trayMenu = new QMenu();
    trayMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayMenu);
    // map the user clicking the tray icon to a function reopening the UI
    connect(trayIcon, &QSystemTrayIcon::activated, this,
            &MainWindow::onTrayIconActivated);
    // map the user clicking the notification to opening the UI
    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, [=](){
        Qui->show();
        Qui->raise();
        Qui->activateWindow();
        if(this->mode != FileManager::Files){
            ShowDupesInUI(*manager, mode);
        }
        this->mode = FileManager::Files;
    });
    this->trayIcon->show();

    // tray icon shit end

    // Map that links directory paths to the corresponding FileOrDownloads type
    std::unordered_map<QString, FileManager::FileOrDownloads> pathToTypeMap = {
        {QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), FileManager::Downloads},
        {QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), FileManager::Desktop},
        {QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), FileManager::Documents},
        {QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), FileManager::Pictures}
    };
    // Download check loop start

    QFileSystemWatcher* watcher = new QFileSystemWatcher(this);
    for (const auto& pathType : pathToTypeMap) {
        watcher->addPath(pathType.first);
    }

    QTimer* debounceTimer = new QTimer(this);
    debounceTimer->setSingleShot(true);
    debounceTimer->setInterval(500);

    // wrapper around file checker to prevent multiple of same calls
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, [=](const QString& path){
        debounceTimer->start();  // Restart the debounce timer
        debounceTimer->setProperty("changedPath", path); // store changed path
    });

    connect(debounceTimer, &QTimer::timeout, this, [=](){
        // If monitor mode is off, ignore everything
        if (!settings.monitorMode) {
            return;
        }
        // Get the changed directory path from the timer property
        QString changedPath = debounceTimer->property("changedPath").toString();

        if (pathToTypeMap.find(changedPath) != pathToTypeMap.end()) {
            // Get the directory type from the map
            FileManager::FileOrDownloads dirType = pathToTypeMap.find(changedPath)->second;

            // check settigns if directory is enabled
            bool allowProcess = false;
            switch (dirType) {
            case FileManager::FileOrDownloads::Downloads:
                allowProcess = settings.downloads;
                break;
            case FileManager::FileOrDownloads::Pictures:
                allowProcess = settings.pictures;
                break;
            case FileManager::FileOrDownloads::Desktop:
                allowProcess = settings.desktop;
                break;
            case FileManager::FileOrDownloads::Documents:
                allowProcess = settings.documents;
                break;
            default:
                break;
            }

            if (!allowProcess) {
                return; // Specific setting is OFF, ignore
            }

            QDir dir(changedPath);
            dir.setFilter(QDir::Files | QDir::NoSymLinks);
            dir.setSorting(QDir::Time | QDir::Reversed);

            QFileInfoList fileList = dir.entryInfoList();

            if (!fileList.isEmpty()) {
                QFileInfo lastFile = fileList.last();
                if (lastFile.suffix() != "tmp") {
                    qDebug() << "NEWFILE:" << lastFile.absoluteFilePath();

                    // Compare the recent file to the other files in the directory
                    // (filters by size and type)
                    for (const QFileInfo& otherFileInfo : fileList) {
                        if (lastFile.absoluteFilePath() == otherFileInfo.absoluteFilePath()) {
                            continue;  // Skip the same file
                        }

                        // Compare by size then suffix
                        if (lastFile.size() == otherFileInfo.size() &&
                            lastFile.suffix() == otherFileInfo.suffix()) {
                            // Create FileInfo for the recent and other file
                            fs::path lastFPath = otherFileInfo.absoluteFilePath().toStdString();
                            FileInfo lastFile(lastFPath, QString::fromStdString(lastFPath.extension().string()), fs::file_size(lastFPath));
                            fs::path otherFPath = otherFileInfo.absoluteFilePath().toStdString();
                            FileInfo otherFile(otherFPath, QString::fromStdString(otherFPath.extension().string()), fs::file_size(otherFPath));

                            if (lastFile == otherFile) {
                                // Duplicate found, add to the corresponding map
                                manager->addFileToTypeSizeMap(otherFile, dirType);
                            }
                        }
                    }

                    // Add the original file for detection
                    fs::path fPath = lastFile.absoluteFilePath().toStdString();
                    FileInfo file(fPath, QString::fromStdString(fPath.extension().string()),
                                  fs::file_size(fPath));

                    manager->addFileToTypeSizeMap(file, dirType);

                    // Check if it's a duplicate
                    if (manager->isDupe(file, dirType)) {
                        ShowNotification("Duplicate Detected, Open ReplicaReaper to delete: ", file.getFileName());
                        this->mode = dirType;
                    }
                }
            }
        }
    });

    // Download check loop end
}


MainWindow::~MainWindow() {
    delete ui;
    delete manager;
    delete tutorial;
}

// Overloaded function that automatically gets called when user closes UI
void MainWindow::closeEvent(QCloseEvent *event) {
    // if the "Run in Background" button is checked,
    if (this->settings.monitorMode) {
        // Program will run in background to check
        this->hide();
        event->ignore();
    } else {
        qApp->quit();  // Close the full application
        event->accept();
    }
}

void MainWindow::setSettings(Window::AppSettings settingsWindow){
    this->settings.monitorMode = settingsWindow.monitorMode;
    this->settings.downloads = settingsWindow.downloads;
    this->settings.desktop = settingsWindow.desktop;
    this->settings.pictures = settingsWindow.pictures;
    this->settings.documents = settingsWindow.documents;
}

/* onReaperButtonClicked(): called when the reaper button is
 * clicked. Prompts user for directory to be reaped. Runs
 * the general program and find all duplicates
 *
 * input: nothing
 * output: nothing
 *
 * NOTE: not sure that we should reset the maps at the
 * end of this. Or atleast we need to make sure that run in
 * the background is not turned on if we do.
 *
 */
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
    ui->CancelBTN->setVisible(true);
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
        // If the cancel button was flagged
        if (this->getCancelButtonState()) {
            this->setCancelButtonState(false);  // reset state
            ui->treeWidget->clear();  // clear table (if any)
            manager->ClearData();  // clear filemanager data
            ui->CancelBTN->setVisible(false);  // show cancel button
            ui->RunReaperBTN->setEnabled(true);  // re-enable reaper button
            ui->progressBar->setValue(0);  // reset the progress bar to 0
            ShowNotification("Reaping Canceled", "Scanning has cancelled sucessfully");
            return;
        }
        // setup for FileInfo class
        fs::path fPath = filePaths[i].toStdString();
        FileInfo file(fPath, QString::fromStdString(fPath.extension().string()),
                    fs::file_size(fPath));
        // Push and sort FileInfo class into FileManager class
        manager->addFileToTypeSizeMap(file, FileManager::Files);  // passes in fileinfo
        // std::cout << *manager;  // DEBUG *************

        if (i % 50 == 0 || i == filePaths.size() - 1) {  // Update every 50 files
            ui->progressBar->setValue(i + 1);              // Update progress bar
            // Process events to keep UI responsive (for progress bar)
            QCoreApplication::processEvents();
        }
        // qDebug() << "File No: " << i;
    }

    // returns elapsed time in milliseconds ( /1000 for seconds)
    auto elapsedTime = timer.elapsed();
    QString message =
        "Took " + QString::number(elapsedTime / 1000.0, 'f') + " seconds";
    // ShowNotification("Hashing Complete", message);
    ShowDupesInUI(*manager, FileManager::Files);
    // re-enable the button
    ui->CancelBTN->setVisible(false);
    ui->RunReaperBTN->setEnabled(true);
    // clear the filemanager maps
    manager->ClearData();
}


/* ShowDupesInUI(): Displays the duplicates to the UI window.
 * Displays filename and the date it was last modified?
 *
 * input: The filemanager holding all duplicates
 * output: UI display with a list of duplcates
 */
void MainWindow::ShowDupesInUI(const FileManager &f, FileManager::FileOrDownloads choice) {
    ui->treeWidget->blockSignals(true);
    // determine which map to iterate over
    //const auto& DupesMap = (choice == FileManager::Files) ? f.Dupes : f.DownloadDupes;

    const std::unordered_map<QByteArray, std::list<FileInfo>> *DupesMap;

    switch (choice) {
    case FileManager::Files:
        DupesMap = &f.Dupes; // Assign f.Dupes map for File case
        break;
    case FileManager::Downloads:
        DupesMap = &f.DownloadDupes; // Assign f.DownloadDupes map for Download case
        break;
    case FileManager::Documents:
        DupesMap = &f.DocumentsDupes;
        break;
    case FileManager::Desktop:
        DupesMap = &f.DesktopDupes;
        break;
    default:
        // Handle invalid or unsupported enum values
        qDebug() << "Unsupported choice for DupesMap";
        ui->treeWidget->blockSignals(false);
        return;
    }

    QString out;
    for (auto it = DupesMap->begin(); it != DupesMap->end(); ++it) {
        // Make a parent item for the list tree widget
        QTreeWidgetItem *parentHashItem = new QTreeWidgetItem(ui->treeWidget);
        parentHashItem->setText(0, it->second.front().getFileName());
        parentHashItem->setText(
            1, QString::fromStdString(
                it->second.front()
                    .getFilePath()
                    .string()));  // Next column value to go under FilePath
        parentHashItem->setText(2, it->second.front().getDate().toString());
        parentHashItem->setToolTip(
            1, QString::fromStdString(
                it->second.front()
                    .getFilePath()
                    .string()));
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
            childItem->setToolTip(1, out);
            childItem->setText(2, a.getDate().toString());
            childItem->setCheckState(0, Qt::Unchecked);
        }
    }

    QHeaderView* header = ui->treeWidget->header();
    // Enable interactive resizing
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    // This allows the second column to be resized
    // in case the user wants to stretch out the filepath column
    auto colWidth = ui->treeWidget->columnWidth(0);
    header->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->treeWidget->setColumnWidth(0, colWidth);

    // Clamps the widget to not stretch beyond the viewport
    connect(ui->treeWidget->header(), &QHeaderView::sectionResized,
            this, [=](int logicalIndex, int oldSize, int newSize) {
                if (logicalIndex == 0 && newSize > colWidth) {
                    ui->treeWidget->setColumnWidth(0, colWidth);
                }
            });
    // helper function to resize widget colums when expanded or collapsed
    auto adjustColumnWidth = [=]() {
        QHeaderView* header = ui->treeWidget->header();
        // Temporarily resize to fit all visible content
        header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        // Lock new width
        int updatedColWidth = ui->treeWidget->columnWidth(0);
        header->setSectionResizeMode(0, QHeaderView::Interactive);
        ui->treeWidget->setColumnWidth(0, updatedColWidth);
    };
    // When a parent is expanded
    connect(ui->treeWidget, &QTreeWidget::itemExpanded,
            this, [=](QTreeWidgetItem *) {
                adjustColumnWidth();
            });
    // When a parent is collapsed
    connect(ui->treeWidget, &QTreeWidget::itemCollapsed,
            this, [=](QTreeWidgetItem *) {
                adjustColumnWidth();
            });
    // Prevent column resizing beyond viewport width
    header->setStretchLastSection(false);
    ui->treeWidget->blockSignals(false);
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
        // special case to uncheck the oringial file when parent in unchecked
        if (newState == Qt::Unchecked) {
            item->child(0)->setCheckState(0, newState);
        }
        // set all the childs states to match the parents
        // (Excluding the original file at index 0)
        for (int i = 1; i < item->childCount(); ++i) {
            item->child(i)->setCheckState(0, newState);
        }
    } else {  // If a child item is toggled
        QTreeWidgetItem *parent = item->parent();
        bool allChecked = true, anyChecked = false;
        // compare all the child elements to see if some, all, or none are checked
        // (Excluding the original file at index 0)
        for (int i = 1; i < parent->childCount(); ++i) {
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
            // special case for if the original file is checked
            auto firstChild = parentItem->child(0);
            if (firstChild->checkState(0) == Qt::Checked) {
                auto filename = firstChild->text(0);
                auto filepath = firstChild->text(1);
                // add pairs to files
                std::pair<QString, QString> pair(filename, filepath);
                files.push_back(pair);
            }
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
        manager->addFileToTypeSizeMap(file, FileManager::Files);
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
void MainWindow::onSettBTN_clicked() {
    Settings *settings = new Settings(this);
    settings->setConfig(this->settings);
    settings->setModal(true);
    settings->exec();
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
    // Get the original file paths from tree widget parents
    std::unordered_set<QString> originalPaths;
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *parentItem = ui->treeWidget->topLevelItem(i);
        originalPaths.insert(parentItem->text(1));
    }
    // Make a list widget to populate files
    QListWidget *listWidget = new QListWidget();
    int originalCount = 0;
    for (const auto &filePair : files) {
        auto& filename = filePair.first;
        auto& filepath = filePair.second;
        QListWidgetItem *item = new QListWidgetItem(filename);

        if (originalPaths.find(filepath) != originalPaths.end()) {
            originalCount++;
            item->setText(filename + " [ORIGINAL]");

            QFont boldFont = item->font();
            boldFont.setBold(true);
            item->setFont(boldFont);
            item->setForeground(Qt::darkRed);  // Optional: visually distinct
        }
        listWidget->addItem(item);
    }

    layout->addWidget(listWidget);

    if (originalCount > 0) {
        label->setTextFormat(Qt::RichText);
        label->setText(label->text() + "<br><b>You have: "
                       + QString::number(originalCount) + " original files selected. </b>");
    }

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No);
    layout->addWidget(buttonBox);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog,
                    &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog,
                   &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        // Delete files
        // Deleting File Logic:
        QStringList failedDeletions;
        for (const auto& filePair : files) {
            QString fullPath = filePair.second;
            QFile file(fullPath);
            if (!file.remove()) {
            failedDeletions.append(fullPath);
            }
        }

        if (!failedDeletions.isEmpty()) {
            QString errorMsg = "Failed to delete the following files:\n"
                               + failedDeletions.join("\n");
            QMessageBox::warning(this, "Deletion Failed", errorMsg);
        } else {
            QMessageBox::information(this, "Success", "Selected files deleted successfully.");
        }
    } else {
        // User Canceled
    }
}

void MainWindow::onHowUseBTN_clicked() {
    if (!tutorial) {
        tutorial = new Tutorial(this);
    }
    tutorial->show();
    tutorial->raise();           // Bring to front
    tutorial->activateWindow();  // Give focus
}

void MainWindow::onCancelBTN_clicked() {
    // flag the cancel boolean
    this->setCancelButtonState(true);
}

///////

void MainWindow::ShowNotification(const QString& title,
                                   const QString& message) {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning("System tray is not available.");
        return;
    }

    // Display the notification | 10000 ms = 10 seconds till timeout
    this->trayIcon->showMessage(title, message, QSystemTrayIcon::Information,
                                10000);
}

// this function activates when the system tray icon is clicked
void MainWindow::onTrayIconActivated(
    QSystemTrayIcon::ActivationReason reason) {
    // if the icon is clicked and the ui exists
    if (reason == QSystemTrayIcon::Trigger && Qui) {
        const FileManager& m = *manager;
        // If the main window is hidden or minimized, show it
        if (Qui->isHidden()) {
            Qui->show();            // Show the window if hidden
            Qui->raise();           // Bring the window to the front
            Qui->activateWindow();  // Give the window focus
            // If background detection is not enabled (Files Mode) dont show found duplicates
            if(this->mode != FileManager::Files){
                ShowDupesInUI(m,this->mode);
            }
        } else {
            Qui->raise();
            Qui->activateWindow();
        }
    }
}
