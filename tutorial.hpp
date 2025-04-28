// Copyright 2025 Replica Reaper
#ifndef TUTORIAL_H
#define TUTORIAL_H

#include <QDialog>
#include <QStandardPaths>
#include <QDir>
#include "filemanager.hpp"
namespace Ui {
class Tutorial;
}

class Tutorial : public QDialog {
    Q_OBJECT

 public:
    explicit Tutorial(QWidget *parent = nullptr);
     void StartDemo(FileManager::FilesOrDownloads mode);
     void ontryDownBTN_clicked();
     void ontryDocBTN_clicked();
     void ontryPicBTN_clicked();
     void ontryDeskBTN_clicked();

     ~Tutorial();

 private:
    Ui::Tutorial *ui;
};

#endif  // TUTORIAL_HPP
