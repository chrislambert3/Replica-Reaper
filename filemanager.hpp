// Copyright 2025 Replica Reaper
#pragma once
#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP
#include <QString>
#include <QWidget>
#include <QObject>
#include <QSystemTrayIcon>
#include <unordered_map>
#include <iostream>
#include <list>
#include "FileInfo.hpp"

using std::unordered_map;

class FileManager : public QObject {
    Q_OBJECT

 public:
    explicit FileManager(QObject* parent = nullptr);
    QString PromptDirectory(QWidget* widget);
    QByteArray HashFile(QString fileName);
    QStringList ListFiles(const QString& directoryPath);
    void AddToDupes(const FileInfo& File);
    void ShowNotification(const QString& title, const QString& message);
    void addFileToList(const FileInfo& file);
    void CheckAndAddDupes(const std::list<FileInfo>& list, const FileInfo& file);
    ~FileManager();

    friend std::ostream& operator<<(std::ostream& out,
                const FileManager& f);

    unordered_map<QString, unordered_map<uintmax_t, std::list<FileInfo>>> AllFilesByTypeSize;

 private:
    QSystemTrayIcon* trayIcon;
    // Map filtered by file type and then file size
    // unordered_map<QString, unordered_map<uintmax_t, std::list<FileInfo>>> AllFilesByTypeSize;
    // Holds list of list of duplicates
    // no particular order
    unordered_map<QByteArray, std::list<FileInfo>> Dupes;
};

#endif /* FILEMANAGER_HPP */
