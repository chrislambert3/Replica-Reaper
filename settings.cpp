// Copyright 2025 Replica Reaper
#include <QMessageBox>
#include "settings.hpp"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) : QDialog(parent), ui(new Ui::Settings) {
  ui->setupUi(this);
}

Settings::~Settings() { delete ui; }

void Settings::on_cancelBTN_clicked() { this->hide(); }

void Settings::on_applyBTN_clicked() {
  applySettings();
  this->hide();
}
void Settings::closeEvent(QCloseEvent *event) {
  // event->accept();
}

void Settings::setState(bool backgroundCheck) {
  ui->bgCheckBox->setCheckState(backgroundCheck ? Qt::Checked : Qt::Unchecked);
}
// applies the changed settings in the mainwindow
void Settings::applySettings() {
  MainWindow *mainWindow = qobject_cast<MainWindow *>(parent());
  if (mainWindow) {
    // Update the parent window's state
    mainWindow->setBackgroundState(ui->bgCheckBox->isChecked());
  }
}
