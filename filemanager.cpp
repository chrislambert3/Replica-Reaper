// Copyright 2025 Replica Reaper
#include "filemanager.hpp"
#include "mainwindow.hpp"

FileManager::FileManager(QObject* parent)
    : QObject(parent), trayIcon(new QSystemTrayIcon()), ui(nullptr) {
  // can also set our custon icon like this:
  // trayIcon->setIcon(QIcon(":/icon.png")); // Set an icon (optional, ensure
  // the path is correct sets a standard tray icon for now
  this->trayIcon->setIcon(
      QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));
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
          &FileManager::onTrayIconActivated);
  this->trayIcon->show();
}
FileManager::~FileManager() {}

QString FileManager::PromptDirectory(QWidget* parent) {
  QString filePath = QFileDialog::getExistingDirectory(
      parent, "Open a Directory", QDir::homePath(),
      QFileDialog::DontResolveSymlinks);

    if (filePath.isEmpty()) {
      qDebug() << "No directory selected";
        return QString();
    }

  qDebug() << "Selected File Path: " << filePath;
  return filePath;
}

QByteArray FileManager::HashFile(QString fileName) {
  // Opens the file
  QFile file(fileName);
  // sets the hash generator
  QCryptographicHash hash(QCryptographicHash::Sha256);

  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Could not open file:" << fileName;
    return QByteArray("");
  }

  if (!hash.addData(&file)) {
    qWarning() << "Could not sha256 hash the file";
    return QByteArray("");
  }

  QByteArray result = hash.result().toHex();
  file.close();
  return result;
}

QStringList FileManager::ListFiles(const QString& directoryPath) {
  QDir dir(directoryPath);
  if (!dir.exists()) {
    qWarning() << "Directory does not exist:" << directoryPath;
    return QStringList();
  }

  QStringList fullPaths;
  QDirIterator it(directoryPath, QDir::Files | QDir::NoDotAndDotDot,
                  QDirIterator::Subdirectories);

  while (it.hasNext()) {
    fullPaths.append(it.next());
  }

  // qDebug() << "Files in directory (recursive):" << fullPaths;
  return fullPaths;
}

void FileManager::addFileToList(FileInfo& file) {
  auto& sizeMap = AllFilesByTypeSize[file.getFileType()];  // list of same size file types
  auto& fileList = sizeMap[file.getFileSize()];  // list of same-sized file infos

  fileList.push_back(file);

  // Check for duplicates (if list has more than one file now)
  if (fileList.size() > 1) CheckAndAddDupes(fileList);
}

void FileManager::CheckAndAddDupes(std::list<FileInfo>& list) {
    // Ensure each file in the list has a valid hash
    UpdateHashes(list);

    list.sort();

    std::list<QByteArray> HashList = StoreUniqueHashes(list);

    // runs through the list and pushes all elements of list
    // matching hash to dlist. If dlist is > 1 then it is pushed
    // to dupe
    AddDupesToMap(list, HashList);
}

void FileManager::UpdateHashes(std::list<FileInfo>& list) {
    for (auto& f : list) {
        if (f.getHash() == DEAD_HASH)
            f.setHash(HashFile(QString::fromStdString(f.getFilePath().string())));
    }
}

std::list<QByteArray> FileManager::StoreUniqueHashes(std::list<FileInfo>& list) {
    // List to hold duplicates
    std::list<QByteArray> uniqueHashes;
    // find and store all unique hashes
    for (auto& fileInfo : list) {
        QByteArray currentHash = fileInfo.getHash();
        // is hash already in hashList?
        auto it = std::find(uniqueHashes.begin(), uniqueHashes.end(), currentHash);
        if (it == uniqueHashes.end()) {
            uniqueHashes.push_back(currentHash);
        }
    }
    return uniqueHashes;
}

void FileManager::AddDupesToMap(std::list<FileInfo>& list, const std::list<QByteArray>& HashList) {
    for (const auto& a : HashList) {
        std::list<FileInfo> dList;
        for (auto& b : list) {
            if (a == b.getHash()) {
                dList.push_back(b);
            }
        }

        if (dList.size() > 1) {
            Dupes[dList.begin()->getHash()] = {dList};
        }
    }
}

std::ostream& operator<<(std::ostream& out, const FileManager& f) {
  for (const auto& [fileType, MapSize] : f.AllFilesByTypeSize) {
    out << "File Type: " << fileType.toStdString() << "\n";
    for (const auto& [fileSize, fileList] : MapSize) {
      out << "  Size: " << fileSize << " bytes\n";
      for (const auto& file : fileList) {
        out << "    - " << file.getFilePath() << "\n";
      }
    }
  }
  return out;
}

void FileManager::ShowNotification(const QString& title,
                                   const QString& message) {
  if (!QSystemTrayIcon::isSystemTrayAvailable()) {
    qWarning("System tray is not available.");
    return;
  }

  // Display the notification | 10000 ms = 10 seconds till timeout
  this->trayIcon->showMessage(title, message, QSystemTrayIcon::Information,
                              10000);
}
// This file should add to the qlist of qlists of FileInfos
// if a duplicate isnt found already in here an exception is thrown

void FileManager::AddToDupes(const FileInfo& File) { return; }
// this function activates when the system tray icon is clicked
void FileManager::onTrayIconActivated(
    QSystemTrayIcon::ActivationReason reason) {
  // if the icon is clicked and the ui exists
  if (reason == QSystemTrayIcon::Trigger && ui) {
    // If the main window is hidden or minimized, show it
    if (ui->isHidden()) {
      ui->show();            // Show the window if hidden
      ui->raise();           // Bring the window to the front
      ui->activateWindow();  // Give the window focus
    }
  }
}
// allows access to show and hide the UI
void FileManager::setMainWindow(QMainWindow* ui) { this->ui = ui; }
