// Copyright 2025 Replica Reaper
#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), manager(new FileManager()) {
  ui->setupUi(this);
  ui->progressBar->setValue(0);
  manager->setMainWindow(this);
  // set the "Run in background" checkbox to true
  ui->checkBox->setChecked(true);
  // this links the button on the UI to the event function
  // "RunReaperBTN" is the name of the variable on the ui
  connect(ui->RunReaperBTN, &QPushButton::clicked, this,
          &MainWindow::onPushButtonClicked);
}

MainWindow::~MainWindow() {
  delete ui;
  delete manager;
}

// Overloaded function that automatically gets called when user closes UI
void MainWindow::closeEvent(QCloseEvent* event) {
  // if the "Run in Background" button is checked,
  if (ui->checkBox->isChecked()) {
    // If checked, just close the window (keeps running in bacnground)
    event->accept();  // Accept the event, which will close the window
  } else {
    qApp->quit();  // Close the full application
    event->accept();
  }
}

void MainWindow::onPushButtonClicked() {
  qDebug() << "Button clicked!";

  QString path = manager->PromptDirectory(this);
  if (path == QString()) {
      qDebug("Please select a directory");
      return;
  }

  QStringList filePaths = manager->ListFiles(path);

  // Set up the progress bar
  ui->progressBar->setValue(0);                   // sets it to 0
  ui->progressBar->setMinimum(0);                 // Min value
  ui->progressBar->setMaximum(filePaths.size());  // # of files to hash

  // Set a timer for hashing all files
  QElapsedTimer timer;
  timer.start();

  // Loop through each file and hash it (prints to console for now)
  for (int i = 0; i < filePaths.size(); ++i) {
    // setup for FileInfo class
    fs::path fPath = filePaths[i].toStdString();
    QDateTime currentDateTime = QDateTime::currentDateTime();
    FileInfo file(fPath, QString::fromStdString(fPath.extension().string()),
                  fs::file_size(fPath), currentDateTime);
    // Push and sort FileInfo class into FileManager class
    manager->addFileToList(file);  // passes in fileinfo
    // std::cout << *manager;  // DEBUG *************

    // Update progress bar
    ui->progressBar->setValue(i + 1);

    // Process events to keep UI responsive (for progress bar)
    QCoreApplication::processEvents();
  }

  // std::cout << *manager;  // DEBUG *************

  // returns elapsed time in milliseconds ( /1000 for seconds)
  auto elapsedTime = timer.elapsed();
  QString message =
      "Took " + QString::number(elapsedTime / 1000.0, 'f') + " seconds";
  manager->ShowNotification("Hashing Complete", message);
  ShowDupesInUI(*manager);
}

// adds one file to qlistwidget
// duplicates will be located adjacently
// currently a stub function for testing
// currently just adds string including filepath and date
// Does not currently locate items adjacently
// NEED TO REFACTOR SO IT TAKES A FILEINFO
void MainWindow::ShowDupesInUI(const FileManager& f) {
  QString out;
  for (auto it = f.Dupes.begin(); it != f.Dupes.end(); ++it) {
    for (auto& a : it->second) {
      out = QString::fromStdString(a.getFilePath().string());
      out.append("     ");
      ui->listWidget->addItem(out);
    }
  }
}
