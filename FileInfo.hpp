#ifndef FILEINFO_H
#define FILEINFO_H
#include <QString>
#include <QDateTime>


class FileInfo{
public:
    FileInfo(const QString& FilePath,
             const QString& FileType,
             const QByteArray& Hash,
             const QDateTime& DateFound):
            _FilePath(FilePath),
            _FileType(FileType),
            _Hash(Hash),
            _DateFound(DateFound){}
    const QString& getFilePath() const { return _FilePath; };
    const QByteArray& getHash() const { return _Hash; };
    const QDateTime& getDate() const { return _DateFound; };

private:
    QString _FilePath;
    QString _FileType;
    QByteArray _Hash;
    QDateTime _DateFound;

};

#endif // FILEINFO_H
