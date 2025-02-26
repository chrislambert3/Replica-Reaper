#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QString>
#include <QWidget>
#include <QObject>
#include <QSystemTrayIcon>

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject* parent = nullptr);
    QString PromptDirectory(QWidget* widget);
    QByteArray HashFile(QString fileName);
    QStringList ListFiles(const QString& directoryPath);
    void ShowNotification(const QString& title, const QString& message);
    ~FileManager();

private:
    QSystemTrayIcon* trayIcon;

};

#endif // FILEMANAGER_H
