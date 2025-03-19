#ifndef FILEINFO_H
#define FILEINFO_H
#include <QString>
#include <QDateTime>
#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;


class FileInfo{
public:
    FileInfo(const fs::path& FilePath,
             const QString& FileType,
             const uintmax_t FileSize,
             const QByteArray& Hash,
             const QDateTime& DateFound):
            _FilePath(FilePath),
            _FileType(FileType),
            _FileSize(FileSize),
            _Hash(Hash),
            _DateFound(DateFound){}
    const fs::path& getFilePath() const { return _FilePath; };
    const QByteArray& getHash() const { return _Hash; };
    const QDateTime& getDate() const { return _DateFound; };
    const QString& getFileType() const { return _FileType; };
    const uintmax_t& getFileSize() const { return _FileSize; };

private:
    fs::path _FilePath;
    QString _FileType;
    uintmax_t _FileSize;
    QByteArray _Hash;
    QDateTime _DateFound;

};

#endif // FILEINFO_H
