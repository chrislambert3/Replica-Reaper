#include "trashmanager.hpp"
#import <Foundation/Foundation.h>

bool TrashManager::moveToTrash(const QString &filePath) {
    // Convert QString to NSString
    NSString *fileNSString = [NSString stringWithUTF8String:filePath.toUtf8().constData()];

    // Get the NSFileManager instance
    NSFileManager *fileManager = [NSFileManager defaultManager];

    // Move file to trash using NSFileManager
    NSError *error = nil;
    BOOL success = [fileManager trashItemAtURL:[NSURL fileURLWithPath:fileNSString] resultingItemURL:nil error:&error];

    // Return whether the move was successful
    return success;
}
