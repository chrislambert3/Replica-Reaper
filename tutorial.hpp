// Copyright 2025 Replica Reaper
#ifndef TUTORIAL_H
#define TUTORIAL_H

#include <QDialog>
#include <QStandardPaths>
#include <QDir>
namespace Ui {
class Tutorial;
}

class Tutorial : public QDialog {
    Q_OBJECT

 public:
    explicit Tutorial(QWidget *parent = nullptr);
    ~Tutorial();

 private:
    Ui::Tutorial *ui;
};

#endif  // TUTORIAL_H
