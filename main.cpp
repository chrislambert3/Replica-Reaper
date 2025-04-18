// Copyright 2025 Replica Reaper
#include "mainwindow.hpp"

namespace fs = std::filesystem;

//  Switch to 1 for testing
//  Switch to 0 for regular program***
#define DEBUG 0

int main(int argc, char* argv[]) {
    if (!DEBUG) {
        QApplication app(argc, argv);
        MainWindow window;
        window.show();
        return app.exec();
    } else if (DEBUG) {
        // This Section is for testing on Machine

        std::vector<QString> list = {
            QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
            QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)};

        QApplication app(argc, argv);
        std::ofstream outFile("Performance.csv",
                          std::ios::app);  // // Open file for writing (will
                                           // create if it doesn't exist)
        if (!outFile) {
            std::cerr << "Error opening the file!" << std::endl;
            return -1;  // Return with error if file cannot be opened
        }
        for (const auto& dir : list) {
            if (!dir.isEmpty()) {  // Ensure the directory path is valid
                qint64 time;
                qint64 size;
                QWidget widget;
                MainWindow window(&widget);
                size = window.getDirectorySize(dir);
                time = window.PythonAutoTestHelper(dir);
                outFile << dir.toStdString() << std::endl;
                outFile << static_cast<int64_t>(time) << std::endl;
                outFile << static_cast<int64_t>(size) << std::endl;
            }
        }
        outFile.close();
        return app.exec();
    }
}
