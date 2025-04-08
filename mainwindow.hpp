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
#include <QDialogButtonBox>
#include <QMessageBox>
#include <filesystem>
#include <iostream>
#include <string>
#include "filemanager.hpp"
#include "FileInfo.hpp"
#include "settings.hpp"

using std::list;
using std::pair;

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
  void ShowDupesInUI(const FileManager &f);
  void showDeleteConfirmation(const list<pair<QString, QString>>& files);
  void onTreeItemChanged(QTreeWidgetItem *item);
  void onDelSelBTN_clicked();
  void onDelAllBTN_clicked();
  list<pair<QString, QString>> getCheckedItems();
  void setBackgroundState(bool state);
  void closeEvent(QCloseEvent *event);
  qint64 PythonAutoTestHelper(QString InputPath);
  qint64 getDirectorySize(const QString &dirPath);
  ~MainWindow();

 private slots:
  void on_SettBTN_clicked();

 private:
  Ui::MainWindow *ui;
  FileManager *manager;
  bool backgroundCheck;
};
#endif /* MAINWINDOW_HPP */
