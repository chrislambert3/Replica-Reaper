// Copyright 2025 Replica Reaper
#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), manager(new FileManager()) {
  ui->setupUi(this);
  ui->progressBar->setValue(0);
  manager->setMainWindow(this);
  // set the "Run in background" checkbox to true
  ui->checkBox->setChecked(true);

  // Tree Widget config:
  ui->treeWidget->setColumnCount(
      2);  // Single column for file names (Poss another col for Date Modified)
  ui->treeWidget->setHeaderLabels(
      {"File Path", "Date Modified"});  // tree columns
  ui->treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  ui->treeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  ui->treeWidget->header()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);  // File path length fits conted
  ui->treeWidget->header()->setSectionResizeMode(
      1, QHeaderView::ResizeToContents);  // Date Modified fits content

  // Button Connections:
  // this links the button on the UI to the event function
  // "RunReaperBTN" is the name of the variable on the ui
  connect(ui->RunReaperBTN, &QPushButton::clicked, this,
          &MainWindow::onPushButtonClicked);
  // Connect signal to handle parent-child checkbox logic
  connect(ui->treeWidget, &QTreeWidget::itemChanged, this,
          &MainWindow::onTreeItemChanged);
  // Connect the "DelSelBTN" to the onDelSelBTN_clicked function
  connect(ui->DelSelBTN, &QPushButton::clicked, this,
          &MainWindow::onDelSelBTN_clicked);
  // Connect the "DelAllBTN" to the onDelAllBTN_clicked function
  connect(ui->DelAllBTN, &QPushButton::clicked, this,
          &MainWindow::onDelAllBTN_clicked);
}

MainWindow::~MainWindow() {
  delete ui;
  delete manager;
}

// Overloaded function that automatically gets called when user closes UI
void MainWindow::closeEvent(QCloseEvent *event) {
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
  // disable the button when clicked
  ui->RunReaperBTN->setEnabled(false);

  QString path = manager->PromptDirectory(this);
  if (path == QString()) {
    qDebug("Please select a directory");
    // re-enable button if no directory selectd
    ui->RunReaperBTN->setEnabled(true);
    return;
  }

  // Set up the progress bar
  ui->progressBar->setValue(0);                   // sets it to 0
  ui->progressBar->setMinimum(0);                 // Min value

  // This will set thr progress bar to "busy" mode
  // The worker will reset the maximum after manager->ListFiles() is called
  ui->progressBar->setMaximum(0);

  // Worker class handles duplicate detection algorithm
  Worker *worker = new Worker(manager,path);
  QThread *workerThread = new QThread(this);  // Create a thread for the worker
  // Run the worker object in the thread
  worker->moveToThread(workerThread);
  // Sets the function to call for when thread starts
  connect(workerThread, &QThread::started, worker, &Worker::processFiles);
  // Sends max progress value from thread to UI
  connect(worker, &Worker::progressMaxUpdate, this, &MainWindow::setProgressMax);
  // Sends the current progress bar value from thread to UI
  connect(worker, &Worker::progressUpdate, this, &MainWindow::updateProgress);
  // Handles what to do before exit (Shows Dupes to the UI)
  connect(worker, &Worker::hashingComplete, this, &MainWindow::handleHashingComplete);
  // Deletes the resources when the thread is finished
  connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);

  // Connect the workerFinished signal to cleanup the thread
  connect(worker, &Worker::workerFinished, workerThread, &QThread::quit); // Clean up thread
  connect(worker, &Worker::workerFinished, worker, &Worker::deleteLater);  // Clean up worker object

  workerThread->start();
}

// adds one file to qlistwidget
// duplicates will be located adjacently
// currently a stub function for testing
// currently just adds string including filepath and date
// Does not currently locate items adjacently
// NEED TO REFACTOR SO IT TAKES A FILEINFO
void MainWindow::ShowDupesInUI(const FileManager &f) {
  QString out;
  for (auto it = f.Dupes.begin(); it != f.Dupes.end(); ++it) {
    // Make a parent item for the list tree widget
    QTreeWidgetItem *parentHashItem = new QTreeWidgetItem(ui->treeWidget);
    parentHashItem->setText(0, QString::fromStdString(it->second.front().getFilePath().string()));
    parentHashItem->setText(
        1, "{Placeholder}");  // Next column value to go under Date Modified
    parentHashItem->setCheckState(0, Qt::Unchecked);  // Default unchecked
    // add the parent item to tree
    ui->treeWidget->addTopLevelItem(parentHashItem);

    for (auto &a : it->second) {
      out = QString::fromStdString(a.getFilePath().string());
      // Logic for adding to list Tree:
      // make a a child for the parent hash item
      QTreeWidgetItem *childItem = new QTreeWidgetItem(parentHashItem);
      childItem->setText(0, out);  // set the filepath
      childItem->setText(
          1, QString("{Date Modified}"));  // Next column value for child item
      childItem->setCheckState(0, Qt::Unchecked);
    }
  }
}

// Function to manage parent-child checkbox behavior
void MainWindow::onTreeItemChanged(QTreeWidgetItem *item) {
  if (!item) return;
  // Temporarily disconnect the signal to avoid recursion
  // (because changing child checkstates in this function triggers it again)
  disconnect(ui->treeWidget, &QTreeWidget::itemChanged, this,
             &MainWindow::onTreeItemChanged);

  // If a parent item check (group header) is toggled,
  if (item->parent() == nullptr) {
    // Get the check status of the parent
    Qt::CheckState newState = item->checkState(0);
    // set all the childs states to match the parents
    for (int i = 0; i < item->childCount(); ++i) {
      item->child(i)->setCheckState(0, newState);
    }
  } else {  // If a child item is toggled
    QTreeWidgetItem *parent = item->parent();
    bool allChecked = true, anyChecked = false;
    // compare all the child elements to see if some, all, or none are checked
    for (int i = 0; i < parent->childCount(); ++i) {
      Qt::CheckState state = parent->child(i)->checkState(0);
      if (state == Qt::Checked) anyChecked = true;
      if (state != Qt::Checked) allChecked = false;
    }

    // Update parent state based on children
    if (allChecked) {
      parent->setCheckState(0, Qt::Checked);
    } else if (anyChecked) {
      parent->setCheckState(0, Qt::PartiallyChecked);
    } else {  // none are checked
      parent->setCheckState(0, Qt::Unchecked);
    }
  }
  // Reconnect the signal after the change is done
  connect(ui->treeWidget, &QTreeWidget::itemChanged, this,
          &MainWindow::onTreeItemChanged);
}

void MainWindow::onDelSelBTN_clicked() {
  // prints selected to console for now
  printCheckedItems();
}
void MainWindow::onDelAllBTN_clicked() {
  // currently selects all buttons

  // Iterate through all top-level parent items
  for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
    QTreeWidgetItem *parentItem = ui->treeWidget->topLevelItem(i);

    // Set the parent item to checked
    parentItem->setCheckState(0, Qt::Checked);
    // Child items should automatically check due to onTreeItemChanged()
  }
}
void MainWindow::printCheckedItems() {
  // iterate though the parent widgets
  for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
    QTreeWidgetItem *parentItem = ui->treeWidget->topLevelItem(i);

    // Skip if the parent is unchecked (so no childs are checked)
    if (parentItem->checkState(0) == Qt::Unchecked) {
      continue;
    }

    // Check the state of the parent item
    if (parentItem->checkState(0) == Qt::Checked) {
      qDebug() << "Checked Parent Group:" << parentItem->text(0);
    } else if (parentItem->checkState(0) == Qt::PartiallyChecked) {
      qDebug() << "Partially Checked Parent Group:" << parentItem->text(0);
    }

    // iterate through child items of the checked parent
    for (int j = 0; j < parentItem->childCount(); ++j) {
      QTreeWidgetItem *childItem = parentItem->child(j);

      // Skip if the child is unchecked (for partially checked cases)
      if (childItem->checkState(0) == Qt::Unchecked) {
        continue;
      } else {  // child is checked
        qDebug() << "Checked Item:" << childItem->text(0);
      }
    }
  }
}
void MainWindow::setProgressMax(int max) {
    ui->progressBar->setMaximum(max);  // Update the progress bar maximum
    QCoreApplication::processEvents();
}
void MainWindow::updateProgress(int progress) {
    ui->progressBar->setValue(progress);
    QCoreApplication::processEvents();
}
void MainWindow::handleHashingComplete(qint64 elapsedTime) {
    QString message =
        "Took " + QString::number(elapsedTime / 1000.0, 'f') + " seconds";
    manager->ShowNotification("Hashing Complete", message);
    ShowDupesInUI(*manager);
    ui->RunReaperBTN->setEnabled(true); // re-enable the button
    // Handle the completion of hashing (notify the user, etc.)
}
