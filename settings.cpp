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
    ui->groupBox->setEnabled(false);
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

void Settings::setConfig(Window::AppSettings settings) {
    auto getMode = [](bool mode){return mode ? Qt::Checked : Qt::Unchecked;};
    ui->monitorCheckBox->setCheckState(getMode(settings.monitorMode));
    ui->downCheckBox->setCheckState(getMode(settings.downloads));
    ui->picCheckBox->setCheckState(getMode(settings.pictures));
    ui->deskCheckBox->setCheckState(getMode(settings.desktop));
    ui->docCheckBox->setCheckState(getMode(settings.documents));
}
// applies the changed settings in the mainwindow
void Settings::applySettings() {
    // Update settignsWin based on current UI values
    auto getState = [](Qt::CheckState state) { return state == Qt::Checked; };
    settignsWin.monitorMode = getState(ui->monitorCheckBox->checkState());
    settignsWin.downloads = getState(ui->downCheckBox->checkState());
    settignsWin.pictures = getState(ui->picCheckBox->checkState());
    settignsWin.desktop = getState(ui->deskCheckBox->checkState());
    settignsWin.documents = getState(ui->docCheckBox->checkState());

    // pass MainWindow new settings
    MainWindow *mainWindow = qobject_cast<MainWindow *>(parent());
    if (mainWindow) {
        mainWindow->setSettings(this->settignsWin);
    }
}
