// Copyright 2025 Replica Reaper
#pragma once
#ifndef FILEINFO_HPP
#define FILEINFO_HPP
#include <QString>
#include <QDateTime>
#include <cstdint>
#include <filesystem>
#include <QFileInfo>
#include <iostream>

inline const QByteArray DEAD_HASH = QByteArray::fromHex("0000000000000000");

namespace fs = std::filesystem;

class FileInfo {
 public:
    FileInfo(const fs::path& FilePath,
             const QString& FileType,
             const uintmax_t FileSize):
            _FilePath(FilePath),
            _FileType(FileType),
            _FileSize(FileSize),
            _Hash(DEAD_HASH),
            _DateCreated(getFileCreationDate(FilePath)) {}
    FileInfo(const fs::path& FilePath,
             const QString& FileType,
             const uintmax_t FileSize,
             const QByteArray& Hash):
        _FilePath(FilePath),
        _FileType(FileType),
        _FileSize(FileSize),
        _Hash(Hash),
        _DateCreated(getFileCreationDate(FilePath)) {}
    const fs::path& getFilePath() const { return _FilePath; }
    QByteArray getHash() { return _Hash; }
    const QDateTime& getDate() const { return _DateCreated; }
    const QString& getFileType() const { return _FileType; }
    const uintmax_t& getFileSize() const { return _FileSize; }

    void setHash(const QByteArray& h) { _Hash = h; }

    friend bool operator<(const FileInfo& a, const FileInfo& b) { return a._Hash < b._Hash; }
    friend bool operator==(const FileInfo& a, const FileInfo& b) { return a._Hash == b._Hash; }

 private:
    fs::path _FilePath;
    QString _FileType;
    uintmax_t _FileSize;
    QByteArray _Hash;
    QDateTime _DateCreated;

    static QDateTime getFileCreationDate(const fs::path& filePath) {
        QFileInfo fileInfo(QString::fromStdString(filePath.string()));
        return fileInfo.birthTime();
    }
};

#endif /* FILEINFO_HPP */
