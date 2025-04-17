// Copyright 2025 Replica Reaper
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
#include <QStandardPaths>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QDir>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "filemanager.hpp"
#include "FileInfo.hpp"
#include "settings.hpp"
#include "tutorial.hpp"

using std::list;
using std::pair;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct AppSettings {
    bool backgroundCheck = false;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    explicit MainWindow(QWidget *parent = nullptr);
    void onReaperButtonClicked();
    void ShowDupesInUI(const FileManager &f);
    void ShowDownloadDupesInUI(const FileManager &f);
    void showDeleteConfirmation(const list<pair<QString, QString>>& files);
    void onTreeItemChanged(QTreeWidgetItem *item);
    void onDelSelBTN_clicked();
    void onDelAllBTN_clicked();
    void onCancelBTN_clicked();
    list<pair<QString, QString>> getCheckedItems();
    void setBackgroundState(bool state) { settings.backgroundCheck = state;}
    bool getBackgroundState() { return settings.backgroundCheck;}
    void setQMainWindow(QMainWindow* Qui) { this->Qui = Qui; }
    void setCancelButtonState(bool state) { cancelButtonState = state; }
    bool getCancelButtonState() const { return cancelButtonState; }
    void ShowNotification(const QString& title, const QString& message);
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void closeEvent(QCloseEvent *event);
    qint64 PythonAutoTestHelper(QString InputPath);
    qint64 getDirectorySize(const QString &dirPath);
    ~MainWindow();

 private slots:
    void onSettBTN_clicked();
    void onHowUseBTN_clicked();

 private:
    Ui::MainWindow *ui;
    FileManager *manager;
    Tutorial *tutorial = nullptr;
    AppSettings settings;
    QSystemTrayIcon* trayIcon;
    QAction* quitAction;
    QMenu* trayMenu;
    QMainWindow* Qui;
    bool cancelButtonState = false;
};
#endif /* MAINWINDOW_HPP */
