#include "filemanager.h"
#include <QString>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QFile>

FileManager::FileManager(QObject* parent) : QObject(parent) {}
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

