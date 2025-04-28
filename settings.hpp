// Copyright 2025 Replica Reaper
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <QDialog>
#include <QEvent>
#include <QMessageBox>
#include "ui_settings.h"
#include "mainwindow.hpp"
#include "appsettings.hpp"

namespace Ui {
class Settings;
}

class Settings : public QDialog {
    Q_OBJECT

 public:
    explicit Settings(QWidget *parent = nullptr);
    void applySettings();
    void setConfig(Window::AppSettings settings);
    Window::AppSettings getConfig() {return this->settignsWin;}
    void onCancelBTN_clicked();
    void onApplyBTN_clicked();
    void closeEvent(QCloseEvent *event);

    ~Settings();
 private slots:

 private:
    Ui::Settings *ui;
    Window::AppSettings settignsWin;
};

#endif  // SETTINGS_HPP
