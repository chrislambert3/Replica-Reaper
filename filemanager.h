#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QString>
#include <QWidget>
#include <QObject>
#include <QSystemTrayIcon>
#include "FileInfo.hpp"

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject* parent = nullptr);
    QString PromptDirectory(QWidget* widget);
    QByteArray HashFile(QString fileName);
    QStringList ListFiles(const QString& directoryPath);
    void AddToDupes(const FileInfo& File);
    void ShowNotification(const QString& title, const QString& message);
    ~FileManager();

private:
    QSystemTrayIcon* trayIcon;
    //Holds list of list of Files
    //Organized by Type
    QList<QList<FileInfo>> AllFilesByType;
    //Holds list of list of duplicates
    //no particular order
    QList<QList<FileInfo>> Dupes;
};

#endif // FILEMANAGER_H
