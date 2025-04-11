// Copyright 2025 Replica Reaper
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <QDialog>
#include <QEvent>
#include <QMessageBox>
#include "mainwindow.hpp"

namespace Ui {
class Settings;
}

class Settings : public QDialog {
    Q_OBJECT

 public:
    explicit Settings(QWidget *parent = nullptr);
    void applySettings();
    void setState(bool backgroundCheck);
    void closeEvent(QCloseEvent *event);

    ~Settings();

 private slots:

    void on_cancelBTN_clicked();

    void on_applyBTN_clicked();

 private:
    Ui::Settings *ui;
};

#endif  // SETTINGS_HPP
