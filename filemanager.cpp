#include "filemanager.h"
#include "mainwindow.h"
#include "FileInfo.hpp"
#include <QString>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QFile>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QStyle>

FileManager::FileManager(QObject* parent)
    : QObject(parent)
    , trayIcon(new QSystemTrayIcon())
{
    //sets a standard tray icon for now
    this->trayIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));
    // can also set our custon icon like this
    //trayIcon->setIcon(QIcon(":/icon.png")); // Set an icon (optional, ensure the path is correct)
    this->trayIcon->show();
}
FileManager::~FileManager(){}

QString FileManager::PromptDirectory(QWidget* parent){
    QString filePath = QFileDialog::getExistingDirectory(parent, "Open a Directory", QDir::homePath(), QFileDialog::DontResolveSymlinks);
    qDebug() <<"Selected File Path: " <<filePath;
    return filePath;
}

QByteArray FileManager::HashFile(QString fileName){
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

    QStringList fileList = dir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    QStringList fullPaths;
    for (int i = 0; i < fileList.size(); ++i) {
        fullPaths.append(dir.absoluteFilePath(fileList[i]));
    }

    qDebug() << "Files in directory:" << fullPaths;
    return fullPaths;
}

void FileManager::addFileToList(const FileInfo& file) {
    // if empty, add item to list
    if (AllFilesByTypeSize.empty()) {
        AllFilesByTypeSize[file.getFileType()][file.getFileSize()] = {file};
    } else { // check for existing type
        auto typeIt = AllFilesByTypeSize.find(file.getFileType());
        if (typeIt != AllFilesByTypeSize.end()) { // type found case
            auto& sizes = typeIt->second;
            auto sizeIt = sizes.find(file.getFileSize());

            if (sizeIt != sizes.end()) {  //  size found case
                sizeIt->second.push_back(file);
                //check file for dupes
                CheckAndAddDupes(sizeIt->second, file);
            } else {  //  size not found case
                sizes[file.getFileSize()] = {file};
            }

        } else {  //  type not found case
            AllFilesByTypeSize[file.getFileType()][file.getFileSize()] = {file};
        }
    }


    //
}
void FileManager::CheckAndAddDupes(const std::list<FileInfo>& list, const FileInfo& file) {
    //check hashes and confirm filepaths not same
    //stub
    return;
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

void FileManager::ShowNotification(const QString& title, const QString& message) {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning("System tray is not available.");
        return;
    }
    // Display the notification
    this->trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 10000); // 10000 ms = 10 seconds till timeout
}
//This file should add to the qlist of qlists of FileInfos
//if a duplicate isnt found already in here an exception is thrown
//
void FileManager::AddToDupes(const FileInfo& File){
    return;
}

