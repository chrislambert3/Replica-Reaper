// Copyright 2025 Replica Reaper
#include "trashmanager.hpp"

bool TrashManager::moveToTrash(const QString &filePath) {
    // converts string to windows native format
    std::wstring path = QDir::toNativeSeparators(filePath).toStdWString();

    // adds a double-null-terminatoin (for file operation)
    wchar_t *file = new wchar_t[path.length() + 2]();
    wcscpy(file, path.c_str());

    SHFILEOPSTRUCTW op = {};  // windows file operation struct (for arguments)
    op.wFunc = FO_DELETE;  // Delete argument
    op.pFrom = file;  // Path(s) to delete
    op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;

    // FOF_ALLOWUNDO: This is the flag that tells Windows to move the file to Recycle Bin
    // FOF_NOCONFIRMATION: No UI confirmation dialogs
    // FOF_SILENT: Suppresses progress UI

    int result = SHFileOperationW(&op);
    delete[] file;
    return (result == 0);
}

