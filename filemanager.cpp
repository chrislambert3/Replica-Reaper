// Copyright 2025 Replica Reaper
#include "filemanager.hpp"
#include "mainwindow.hpp"

FileManager::FileManager(QObject* parent) : QObject(parent) {}
FileManager::~FileManager() {}
/* PromptDirectory(): Prompts a request to the user
 * to select a directory to be reaped. Is called
 * within the MainWindow class. Specifically member
 * function OnReaperButtonClicked(mainwindow.cpp).
 *
 * input: the FileManager member value within mainwindow.hpp "manager"
 * output: QString of the directory selected
 */
QString FileManager::PromptDirectory(QWidget* parent) {
    QString filePath = QFileDialog::getExistingDirectory(
        parent, "Open a Directory", QDir::homePath(),
        QFileDialog::DontResolveSymlinks);

    if (filePath.isEmpty()) {
        qDebug() << "No directory selected";
        return QString();
    }

    QDir dir(filePath);
    if (dir.isRoot()) {
        qDebug() << "Root directory selection is not allowed.";
        return QString();
    }

    qDebug() << "Selected File Path: " << filePath;
    return filePath;
}

/* HashFile(): Hashes a single file using Blake2b_160
 *
 * input: file location (QString)
 * output: Hash of the specified file.
 *        -returns QByteArray("") in case of error.
 */
QByteArray FileManager::HashFile(QString fileName) {
    // Opens the file
    QFile file(fileName);
    // sets the hash generator
    QCryptographicHash hash(QCryptographicHash::Blake2b_160);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file:" << fileName;
        return QByteArray("");
    }

    if (!hash.addData(&file)) {
        qWarning() << "Could not Blake2b_160 hash the file";
        return QByteArray("");
    }

    QByteArray result = hash.result().toHex();
    file.close();
    return result;
}

/* ListFiles(): collects all files in a directory and
 * its subdirectories.
 *
 * input: A file directory location
 * output: A QStringList of every file in the input Directory
 */
QStringList FileManager::ListFiles(const QString& directoryPath) {
    QDir dir(directoryPath);
    if (!dir.exists()) {
        qWarning() << "Directory does not exist:" << directoryPath;
        return QStringList();
    }

    QStringList fullPaths;
    QDirIterator it(directoryPath, QDir::Files | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        fullPaths.append(it.next());
    }

    // qDebug() << "Files in directory (recursive):" << fullPaths;
    return fullPaths;
}
/* isDupe(): checks if a file is a duplicate
 *
 * input: File to check and which map to check
 * output: True if file is a dupe
 *         false if file is NOT a dupe
 */
bool FileManager::isDupe(FileInfo& file, FileOrDownloads ChooseWhich) {
    QByteArray FileHash = HashFile(QString::fromStdString(file.getFilePath().string()));
    if (ChooseWhich == Files) {
        auto it = Dupes.find(FileHash);
        return (it != Dupes.end()) ? true : false;
    } else if (ChooseWhich == Downloads) {
        auto it = DownloadDupes.find(FileHash);
        return (it != DownloadDupes.end()) ? true : false;
    }
    qDebug() << "error in isDupe()";
    return false;
}

/* AddFileToTypeSizeMap(): Adds a single file to the data member
 * "AllFilesByTypeSize" located in the File manager class. Correctly
 * sorts the file to its correct location within the map. First
 *
 * input: a single file location
 * output: an updated AllFilesByTypeSize map containing
 * that input file.
 */
void FileManager::addFileToTypeSizeMap(FileInfo& file, FileOrDownloads ChooseWhich) {
    // key == input files type
    // value == size map for that type
    if (ChooseWhich == Files) {
            // key == input files type
            // value == size map for that type
        auto& sizeMap =
            AllFilesByTypeSize[file.getFileType()];
        // key == input file size
        // value = list for that type and size
        auto& fileList =
            sizeMap[file.getFileSize()];

        fileList.push_back(file);

        // if list has > 1 size, check dupes and hash files
        // for later duplicate detection.
        if (fileList.size() > 1) CheckAndAddDupes(fileList, Files);
    } else if (ChooseWhich == Downloads) {
        auto& sizeMap =
            AllDownloadsByTypeSize[file.getFileType()];
        auto& fileList =
            sizeMap[file.getFileSize()];

        fileList.push_back(file);

        if (fileList.size() > 1) CheckAndAddDupes(fileList, Downloads);
    }
}
/* CheckAndAddDupes(): Checks a list of FileInfo Objects
 * for duplicates and adds them to the DupesMap if they
 * are found.
 *
 * input: List of FileInfo objects, with a sepcific Type and Size
 * output: an updated DupesMap.
 */
void FileManager::CheckAndAddDupes(std::list<FileInfo>& list, FileOrDownloads ChooseWhich) {
    // File Hashes will be needed for duplicate detection
    UpdateHashes(list);

    // Makes finding dupes easier
    list.sort();

    // Hash map implementation:
    std::unordered_map<QByteArray, std::list<FileInfo>> hashToFilesMap;

    // convert the FileInfo list into a map with the file hash as the key
    for (auto& file : list) {
        // hash key
        const QByteArray& hash = file.getHash();
        hashToFilesMap[hash].push_back(file);
    }
    // Iterate through the of hash keys and
    // add only duplicates (lists with more than 1 file)
    for (auto& [hash, dList] : hashToFilesMap) {
        if (dList.size() > 1) {
            // move original item to 1st element in list
            auto earliest = std::min_element(
                dList.begin(), dList.end(), [](const FileInfo& l, const FileInfo& r) {
                    return l.getDate() < r.getDate();
                });

            if (earliest != dList.end()) {
                dList.splice(dList.begin(), dList, earliest);
            }
            if (ChooseWhich == Files) {
                Dupes[hash] = dList;
            } else if (ChooseWhich == Downloads) {
                DownloadDupes[hash] = dList;
            }
        }
    }
}

/* UpdateHashes(): finds the hash value for FileInfo objects
 * in a list if they are not already hashed.
 *
 * input: List of FileInfo objects
 * output: An updated List of FileInfo objects
 */
void FileManager::UpdateHashes(std::list<FileInfo>& list) {
    for (auto& f : list) {
        if (f.getHash() == DEAD_HASH)
            f.setHash(HashFile(QString::fromStdString(f.getFilePath().string())));
    }
}

/* operator<<(): overloaded out operator for the Filemanager
 * class. Mainly used for debugging
 *
 * input: FileManager object
 * output: Files by type and size in the outstream
 *          specified
 */
std::ostream& operator<<(std::ostream& out, const FileManager& f) {
    for (const auto& [fileType, MapSize] : f.AllFilesByTypeSize) {
        out << "File Type: " << fileType.toStdString() << "\n";
        for (const auto& [fileSize, fileList] : MapSize) {
            out << "  Size: " << fileSize << " bytes\n";
            for (const auto& file : fileList) {
                out << "    - " << file.getFilePath() << "\n";
            }
        }
    }
  return out;
}

