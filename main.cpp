// Copyright 2025 Replica Reaper
#include "mainwindow.hpp"
#include <vector>
#include <QString>
#include <string>
#include <fstream>

namespace fs = std::filesystem;

//  Switch to 1 for testing
//  currently hardcoded to Braden's machine
#define DEBUG 0


int main(int argc, char *argv[]) {
    if(!DEBUG) {
        QApplication app(argc, argv);
        QWidget widget;
        MainWindow window(&widget);
        window.show();
        return app.exec();
    } else if(DEBUG) {
        // This Section is for testing on Braden's Machine
        QApplication app(argc, argv);
        std::vector<std::string> list = {"C:\\Users\\brade\\Downloads", "C:\\Users\\brade\\Documents", "C:\\Users\\brade\\Desktop", "C:\\Users\\brade\\Pictures"};
        std::ofstream outFile("Performance.csv", std::ios::app);  // Open file for writing (will create if it doesn't exist)
        if (!outFile) {
            std::cerr << "Error opening the file!" << std::endl;
            return -1;  // Return with error if file cannot be opened
        }
        for(int i = 0; i < list.size(); ++i){
            qint64 time;
            qint64 size;
            QWidget widget;
            MainWindow window(&widget);
            size = window.getDirectorySize(QString::fromUtf8(list[i]));
            time = window.PythonAutoTestHelper(QString::fromUtf8(list[i]));
            outFile << list[i] << std::endl;
            outFile << static_cast<long long>(time) << std::endl;
            outFile << static_cast<long long>(size) << std::endl;
        }
        return app.exec();
    }
}
