// Copyright 2025 Replica Reaper
#include "mainwindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget widget;
    MainWindow window(&widget);
    window.show();

    return app.exec();
}
