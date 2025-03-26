// Copyright 2025 Replica Reaper
#pragma once
#ifndef FILEINFO_HPP
#define FILEINFO_HPP
#include <QString>
#include <QDateTime>
#include <cstdint>
#include <filesystem>

inline const QByteArray DEAD_HASH = QByteArray::fromHex("0000000000000000");

namespace fs = std::filesystem;

class FileInfo{
 public:
    FileInfo(const fs::path& FilePath,
             const QString& FileType,
             const uintmax_t FileSize,
             const QDateTime& DateFound):
            _FilePath(FilePath),
            _FileType(FileType),
            _FileSize(FileSize),
            _Hash(DEAD_HASH),
            _DateFound(DateFound) {}
    const fs::path& getFilePath() const { return _FilePath; }
    QByteArray getHash() { return _Hash; }
    const QDateTime& getDate() const { return _DateFound; }
    const QString& getFileType() const { return _FileType; }
    const uintmax_t& getFileSize() const { return _FileSize; }

    void setHash(const QByteArray& h) { _Hash = h; }

 private:
    fs::path _FilePath;
    QString _FileType;
    uintmax_t _FileSize;
    QByteArray _Hash;
    QDateTime _DateFound;
};

#endif /* FILEINFO_HPP */
