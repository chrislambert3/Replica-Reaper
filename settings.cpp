// Copyright 2025 Replica Reaper
#include "settings.hpp"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) : QDialog(parent), ui(new Ui::Settings) {
    ui->setupUi(this);

    // Connect apply and cancel buttons to functions
    connect(ui->applyBTN, &QPushButton::clicked, this,
            &Settings::onApplyBTN_clicked);
    connect(ui->cancelBTN, &QPushButton::clicked, this,
            &Settings::onCancelBTN_clicked);
    // monitor check box function
    connect(ui->monitorCheckBox, &QCheckBox::checkStateChanged, this, [=](int state){
        ui->groupBox->setEnabled(state == Qt::Checked);
    });
}

Settings::~Settings() { delete ui; }

void Settings::onCancelBTN_clicked() { this->hide(); }

void Settings::onApplyBTN_clicked() {
    applySettings();
    this->hide();
}
void Settings::closeEvent(QCloseEvent *event) {
    // event->accept();
}

void Settings::setState(bool backgroundCheck) {
    ui->bgCheckBox->setCheckState(backgroundCheck ? Qt::Checked : Qt::Unchecked);
}
bool Settings::getState() {
    return ui->bgCheckBox->isChecked();
}
// applies the changed settings in the mainwindow
void Settings::applySettings() {
    MainWindow *mainWindow = qobject_cast<MainWindow *>(parent());
    if (mainWindow) {
        // Update the parent window's state
        mainWindow->setBackgroundState(ui->bgCheckBox->isChecked());
    }
}
