// Copyright 2025 Replica Reaper
#ifndef WORKER_HPP
#define WORKER_HPP

#include <QObject>
#include <QStringList>
#include <QFileInfoList>
#include "filemanager.hpp"

class Worker : public QObject {
    Q_OBJECT

public:
    Worker(FileManager *manager, const QString &path, QObject *parent = nullptr)
        : QObject(parent), m_manager(manager), m_path(path) {}
    ~Worker(){}

signals:
    void progressMaxUpdate(int max);    // Signal to set max value of progress bar
    void progressUpdate(int progress);  // Signal to update progress bar
    void hashingComplete(qint64 elapsed); // Signal to trigger the notification
    void workerFinished(); // Signal to handle closing the worker after

public slots:
    void processFiles(){
        // Call ListFiles() to get file paths and emit the total file count
        QStringList filePaths = m_manager->ListFiles(m_path);
        int totalFiles = filePaths.size();
        qDebug() <<"Total amount files to cover: " << totalFiles;
        // Emit the total file count to update the progress bar max value
        emit progressMaxUpdate(totalFiles);
        // Set a timer for hashing all files
        QElapsedTimer timer;
        timer.start();

        // Iterate through each file and process it
        for (int i = 0; i < totalFiles; ++i) {
            QString filePath = filePaths[i];

            fs::path fPath = filePath.toStdString();
            FileInfo file(fPath, QString::fromStdString(fPath.extension().string()),
                          fs::file_size(fPath));
            m_manager->addFileToList(file);

            // Update every 50 files (or at 100%)
            if (i % 50 == 0 || i == totalFiles - 1){
                emit progressUpdate(i + 1);  // Emit signals to update progress
            }
        }
        auto elapsedTime = timer.elapsed();

        emit hashingComplete(elapsedTime);  // Notify when all files are processed
        emit workerFinished(); // Notify when to close the thread
    }

private:
    FileManager *m_manager;  // Pointer to the FileManager
    QString m_path;  // Path to be processed
};

#endif // WORKER_HPP
