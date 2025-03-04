#include "mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget widget;
    MainWindow window(&widget);
    window.show();

    return app.exec();
}
