// Copyright 2021 Replica Reaper
#ifndef TRASHMANAGER_HPP
#define TRASHMANAGER_HPP

#include <QString>
#include <QDir>
#include <windows.h>
#include <shellapi.h>

namespace TrashManager {
bool moveToTrash(const QString &filePath);
}

#endif  // TRASHMANAGER_HPP
