// Copyright 2025 Replica Reaper
#pragma once
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "filemanager.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    explicit MainWindow(QWidget *parent = nullptr);
    void onPushButtonClicked();
    void ShowDupesInUI(const FileManager& f);
    void closeEvent(QCloseEvent *event);
    ~MainWindow();

 private:
    Ui::MainWindow *ui;
    FileManager *manager;
};
#endif /* MAINWINDOW_HPP */
