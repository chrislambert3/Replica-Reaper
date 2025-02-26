#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QString>
#include <QWidget>
#include <QObject>

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject* parent = nullptr);
    QString PromptDirectory(QWidget* widget);
    QByteArray HashFile(QString fileName);
    QStringList ListFiles(const QString& directoryPath);

    ~FileManager();

private:

};

#endif // FILEMANAGER_H
