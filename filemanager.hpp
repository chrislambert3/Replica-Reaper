// Copyright 2025 Replica Reaper
#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP
#include <QString>
#include <QWidget>
#include <QObject>
#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QFile>
#include <QApplication>
#include <QStyle>
#include <QMenu>
#include <QDirIterator>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <list>
#include <string>
#include <algorithm>
#include "FileInfo.hpp"

using std::unordered_map;
using std::unordered_set;

class FileManager : public QObject {
    Q_OBJECT

 public:
    explicit FileManager(QObject* parent = nullptr);
    QString PromptDirectory(QWidget* widget);
    QByteArray HashFile(QString fileName);
    QStringList ListFiles(const QString& directoryPath);
    void AddToDupes(const FileInfo& File);
    void ShowNotification(const QString& title, const QString& message);
    void addFileToTypeSizeMap(FileInfo& file);
    void CheckAndAddDupes(std::list<FileInfo>& list);
    void setMainWindow(QMainWindow* ui);
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void UpdateHashes(std::list<FileInfo>& list);
    void ClearData() {
        AllFilesByTypeSize.clear();
        Dupes.clear();
    }
    ~FileManager();

    friend std::ostream& operator<<(std::ostream& out, const FileManager& f);

    // Map filtered by file type and then file size
    // unordered_map<QString, unordered_map<uintmax_t, std::list<FileInfo>>>
    // AllFilesByTypeSize; Holds list of list of duplicates no particular order
    unordered_map<QString, unordered_map<uintmax_t, std::list<FileInfo>>>
        AllFilesByTypeSize;
    unordered_map<QByteArray, std::list<FileInfo>> Dupes;

 private:
    QSystemTrayIcon* trayIcon;
    QAction* quitAction;
    QMenu* trayMenu;
    QMainWindow* ui;
};

#endif /* FILEMANAGER_HPP */
