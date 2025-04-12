// Copyright 2025 Replica Reaper
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <QDialog>
#include <QEvent>
#include <QMessageBox>
#include "mainwindow.hpp"
#include "ui_settings.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog {
    Q_OBJECT

 public:
    explicit Settings(QWidget *parent = nullptr);
    void applySettings();
    void setState(bool backgroundCheck);
    bool getState();
    void onCancelBTN_clicked();
    void onApplyBTN_clicked();
    void closeEvent(QCloseEvent *event);

    ~Settings();
 private slots:

 private:
    Ui::Settings *ui;
};

#endif  // SETTINGS_HPP
