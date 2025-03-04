#include "filemanager.hpp"
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

void FileManager::ShowNotification(const QString& title, const QString& message) {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning("System tray is not available.");
        return;
    }
    // Display the notification
    this->trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 10000); // 10000 ms = 10 seconds till timeout
}

