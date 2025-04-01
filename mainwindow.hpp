// Copyright 2025 Replica Reaper
#pragma once
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QCloseEvent>
#include <QString>
#include <QListWidget>
#include <QDateTime>
#include <QApplication>
#include <QTreeWidget>
#include <QThread>
#include <filesystem>
#include <iostream>
#include <string>
#include "filemanager.hpp"
#include "FileInfo.hpp"
#include "worker.hpp"

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
    void onTreeItemChanged(QTreeWidgetItem *item);
    void onDelSelBTN_clicked();
    void onDelAllBTN_clicked();
    void printCheckedItems();
    void setProgressMax(int max);
    void updateProgress(int progress);
    void handleHashingComplete(qint64 elapsedTime);
    void closeEvent(QCloseEvent *event);
    ~MainWindow();

 private:
    Ui::MainWindow *ui;
    FileManager *manager;
};
#endif /* MAINWINDOW_HPP */
