// Copyright 2025 Replica Reaper
#include <QCoreApplication>
#include <QTest>
#include <QTemporaryFile>
#include <QProgressBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTextBrowser>
#include <filesystem>
#include <string>
#include "../filemanager.hpp"
#include "../FileInfo.hpp"
#include "../mainwindow.hpp"
// add necessary includes here

/*
Built in function calls:
initTestCase() will be called before the first test function is executed.
<insert testnamefunction>_data() will be called to create a global test data
table. cleanupTestCase() will be called after the last test function was
executed. init() will be called before each test function is executed. cleanup()
will be called after every test function.
 */

class FileManagerTest : public QObject {
  Q_OBJECT

 public:
  explicit FileManagerTest(QObject *parent = nullptr);
  ~FileManagerTest();

 private slots:
  // creates a directory "TestFiles" as well as 3 files in the direcotry
  void initTestCase();  // initTestCase() will be called before any test
                        // function is executed.

  // <testnamefunction>_data() will be called to create a global test data
  // table. Ex. testHashFile_data() will get called inside testHashFile() before
  // anything
  // File Manager Tests
  void testHashFile();
  void testHashFile_data();
  void testHashFile_FileNotFound();
  void testListFilesFail();
  void testInitAddFileToList();
  void testAddDupeToAddFileToList();
  void testAddNonDupeToaddFileToTypeSizeMap();

  // UI Components Tests:
  void testProgressBarUpdate();
  void testTreeWidgetHierarchy();
  void testCheckingParentMarksAllChildren();       // Tree Widget test
  void testUncheckingChildSetsParentPartial();     // Tree Widget test
  void testUncheckingAllChildSetsParentUncheck();  // Tree Widget test

  // FileInfo class Tests:
  void testFileInfoConstruction_WithoutHash();
  void testFileInfoConstruction_WithHash();
  void testFileInfoGetters();
  void testFileInfoSetHash();
  void testFileInfoComparisonOperators();
  void testFileInfoDateCreated();

  // Tutorial class Tests:
  void test_TutorialTextBrowserContent();
  void test_TutorialExternalLinksEnabled();
  void test_TutorialButtonsCreation();

  // Settings class Tests:
  void test_SettingsComponentCreation();
  void test_SettingsApplySettings();
  void test_SettingsCancelButtonClicked();
  void test_SettingsApplyButtonClicked();
  void test_SettingsSetGetState();




  // cleanupTestCase() will be called after the last test function was executed.
  void cleanupTestCase();

 private:
  FileManager testManager;
};

FileManagerTest::FileManagerTest(QObject *parent) : QObject(parent) {}
FileManagerTest::~FileManagerTest() {}

void FileManagerTest::initTestCase() {  // will be called before the first test
  qDebug("Called before everything else.");
  // Get the base path of the current application directory
  QString basePath = QCoreApplication::applicationDirPath();
  QDir testDir(basePath);  // Base directory where app is running

  // *****create a new directory called TestFiles and appended 3 files:

  // Ensure the TestFiles directory exists
  QString testFilesPath = testDir.filePath("TestFiles");
  // Make the directory it if it doesn't exist
  if (!QDir(testFilesPath).exists()) {
    QVERIFY(QDir().mkpath(testFilesPath));  // Create the TestFiles directory
  }

  // Create the test files with some known content
  QFile file1(testFilesPath + "/testfile1.txt");
  if (file1.open(QIODevice::WriteOnly)) {
    file1.write("Hello, World!");  // Known content
    file1.close();
  }

  QFile file2(testFilesPath + "/testfile2.txt");
  if (file2.open(QIODevice::WriteOnly)) {
    file2.write("Another test file.");  // Known content
    file2.close();
  }

  QFile file3(testFilesPath + "/testfile3.txt");
  if (file3.open(QIODevice::WriteOnly)) {
    file3.write("Hello, World!");  // Known content
    file3.close();
  }

  QFile emptyFile(testFilesPath + "/empty.txt");
  emptyFile.open(QIODevice::WriteOnly);
  emptyFile.close();
}

// This creates a data table scoped to only testHashFile() test function ONLY
// Since theres 3 Rows testHashFile() will run 3 times with 3 entries
void FileManagerTest::testHashFile_data() {
  QTest::addColumn<QString>("filePath");
  QTest::addColumn<QByteArray>("expectedHash");

  QString basePath = QCoreApplication::applicationDirPath();
  QDir testDir(basePath);
  QString testFilesPath = testDir.filePath("TestFiles");

  // Add test data for files and their expected hash values
  // *newRow(" custom test case name") << ** "filePath" column entry ** << **
  // "expected
  QTest::newRow("Testing File Hash: test file 1")
      << testFilesPath + "/testfile1.txt"
      << QByteArray(
             "522f974c6500eb7923c28e3b129b52a79405f0fa");
  QTest::newRow("Testing File Hash: test file 2")
      << testFilesPath + "/testfile2.txt"
      << QByteArray(
             "1278f1b6fde6bda8b911ffb89b3586ca7b73c6a9");
  QTest::newRow("Testing File Hash: empty file")
      << testFilesPath + "/empty.txt"
      << QByteArray(
             "3345524abf6bbe1809449224b5972c41790b6cf2");
  qDebug() << "Hash file data table created";
}

// Test function that compares file hashes
void FileManagerTest::testHashFile() {
  // QFETCH is how it pulls the data from the testHashFile_data() table,
  // will automatically run all rows
  QFETCH(QString, filePath);
  QFETCH(QByteArray, expectedHash);

  QByteArray actualHash =
      testManager.HashFile(filePath);  // Call the method to get the hash
  // qDebug() << "Path: " <<filePath;
  // qDebug() << "Hash: " << actualHash;
  QCOMPARE(actualHash, expectedHash);  // Compare actual hash with expected hash
}

// this should trigger
void FileManagerTest::testHashFile_FileNotFound() {
  QTest::ignoreMessage(QtWarningMsg,
                       "Could not open file: \"non_existent_file.txt\"");
  QByteArray result = testManager.HashFile("non_existent_file.txt");
  QVERIFY(result.isEmpty());
}

// Test to check invalid directory path returns a null list
// test invalid directory returns empty QStringList()
void FileManagerTest::testListFilesFail() {
  qDebug("testListFilesFail test.");

  // ignore warning for non-existent directory
  QTest::ignoreMessage(
      QtWarningMsg, "Directory does not exist: \"/path/to/invalid/directory\"");

  QStringList result = testManager.ListFiles("/path/to/invalid/directory");
  QCOMPARE(result.isEmpty(), true);
}

void FileManagerTest::testInitAddFileToList() {
    qDebug("testAddFileToList adding first file test.");

    // create test files to ensure its added correctly
    fs::path fPath = fs::current_path() / "TestFiles/testfile1.txt";
    FileInfo file(fPath, QString::fromStdString(fPath.extension().string()),
                  fs::file_size(fPath),
                  testManager.HashFile("TestFiles/testfile1.txt"));

    // add test file to list
    testManager.addFileToTypeSizeMap(file);

    // verify file type has 1 entry
    QVERIFY(testManager.AllFilesByTypeSize[file.getFileType()][file.getFileSize()]
                .size() == 1);

    // verify file matches
    QCOMPARE(
        testManager.AllFilesByTypeSize[file.getFileType()][file.getFileSize()]
            .front()
            .getFilePath(),
        file.getFilePath());
}

// Test to check files added to list
void FileManagerTest::testAddDupeToAddFileToList() {
  // create test files to ensure its added correctly
  fs::path fPath = fs::current_path() / "TestFiles/testfile1.txt";
  FileInfo file(fPath, QString::fromStdString(fPath.extension().string()),
                fs::file_size(fPath),
                testManager.HashFile("TestFiles/testfile1.txt"));

  fs::path fPathDupe = fs::current_path() / "TestFiles/testfile3.txt";
  FileInfo fileDupe(
      fPathDupe, QString::fromStdString(fPathDupe.extension().string()),
      fs::file_size(fPathDupe), testManager.HashFile("TestFiles/testfile3.txt"));

  fs::path fPathDiff = fs::current_path() / "TestFiles/testfile2.txt";
  FileInfo fileDiff(
      fPathDiff, QString::fromStdString(fPathDiff.extension().string()),
      fs::file_size(fPathDiff), testManager.HashFile("TestFiles/testfile2.txt"));

  qDebug("testAddFileToList verifying duplicate goes to the same list");

  // add test file to list
  testManager.addFileToTypeSizeMap(fileDupe);

  // verify type and size have 2 entries
  QVERIFY(testManager.AllFilesByTypeSize[file.getFileType()][file.getFileSize()]
              .size() == 2);
}

void FileManagerTest::testAddNonDupeToaddFileToTypeSizeMap() {
    // create test files to ensure its added correctly
    fs::path fPath = fs::current_path() / "TestFiles/testfile1.txt";
    FileInfo file(fPath, QString::fromStdString(fPath.extension().string()),
                  fs::file_size(fPath),
                  testManager.HashFile("TestFiles/testfile1.txt"));

    fs::path fPathDupe = fs::current_path() / "TestFiles/testfile3.txt";
    FileInfo fileDupe(
        fPathDupe, QString::fromStdString(fPathDupe.extension().string()),
        fs::file_size(fPathDupe), testManager.HashFile("TestFiles/testfile3.txt"));

    fs::path fPathDiff = fs::current_path() / "TestFiles/testfile2.txt";
    FileInfo fileDiff(
        fPathDiff, QString::fromStdString(fPathDiff.extension().string()),
        fs::file_size(fPathDiff), testManager.HashFile("TestFiles/testfile2.txt"));

    qDebug(
        "testaddFileToTypeSizeMap verifying non-duplicate goes to different size list");

    // add different test file to list
    testManager.addFileToTypeSizeMap(fileDiff);

    // test files at different location
    QVERIFY(testManager.AllFilesByTypeSize[file.getFileType()][file.getFileSize()]
                .size() == 2);
    QVERIFY(
        testManager
            .AllFilesByTypeSize[fileDiff.getFileType()][fileDiff.getFileSize()]
            .size() == 1);
    QVERIFY(file.getHash() != fileDiff.getHash());  // verify different hash
}

void FileManagerTest::testProgressBarUpdate() {
  // Simulate setting progress bar values
  QProgressBar progressBar;
  QProgressBar *pointer = &progressBar;

  QVERIFY(pointer != nullptr);

  progressBar.setValue(0);
  QCOMPARE(progressBar.value(), 0);

  progressBar.setValue(50);
  QCOMPARE(progressBar.value(), 50);

  progressBar.setValue(100);
  QCOMPARE(progressBar.value(), 100);
}

void FileManagerTest::testFileInfoConstruction_WithoutHash() {
  fs::path fPath = "TestFiles/testfile1.txt";
  QString fType = QString::fromStdString(fPath.extension().string());
  uintmax_t fSize = fs::file_size(fPath);

  FileInfo fileInfo(fPath, fType, fSize);

  QCOMPARE(fileInfo.getFilePath(), fPath);
  QCOMPARE(fileInfo.getFileType(), fType);
  QCOMPARE(fileInfo.getFileSize(), fSize);
  QCOMPARE(fileInfo.getHash(), DEAD_HASH);
}

void FileManagerTest::testFileInfoConstruction_WithHash() {
  fs::path fPath = "TestFiles/testfile1.txt";
  QString fType = QString::fromStdString(fPath.extension().string());
  uintmax_t fSize = fs::file_size(fPath);
  QByteArray hash = QByteArray::fromHex("1234567890abcdef");

  FileInfo fileInfo(fPath, fType, fSize, hash);

  QCOMPARE(fileInfo.getFilePath(), fPath);
  QCOMPARE(fileInfo.getFileType(), fType);
  QCOMPARE(fileInfo.getFileSize(), fSize);
  QVERIFY(fileInfo.getHash() != DEAD_HASH);
  QCOMPARE(fileInfo.getHash(), hash);
}

void FileManagerTest::testFileInfoGetters() {
  fs::path fPath = "TestFiles/testfile1.txt";
  QString fType = QString::fromStdString(fPath.extension().string());
  uintmax_t fSize = fs::file_size(fPath);

  FileInfo fileInfo(fPath, fType, fSize);

  QCOMPARE(fileInfo.getFilePath(), fPath);
  QCOMPARE(fileInfo.getFileType(), fType);
  QCOMPARE(fileInfo.getFileSize(), fSize);
  QCOMPARE(fileInfo.getHash(), DEAD_HASH);
}

void FileManagerTest::testFileInfoSetHash() {
  fs::path fPath = "TestFiles/testfile1.txt";
  QString fType = QString::fromStdString(fPath.extension().string());
  uintmax_t fSize = fs::file_size(fPath);
  QByteArray hash = QByteArray::fromHex("1234567890abcdef");

  FileInfo fileInfo(fPath, fType, fSize);

  fileInfo.setHash(hash);
  QVERIFY(fileInfo.getHash() != DEAD_HASH);
  QCOMPARE(fileInfo.getHash(), hash);
}

void FileManagerTest::testFileInfoComparisonOperators() {
  fs::path filePath1 = "TestFiles/testfile1.txt";
  fs::path filePath2 = "TestFiles/testfile2.txt";
  QString fileType = ".txt";
  uintmax_t fileSize = 1024;
  QByteArray hash1 = QByteArray::fromHex("1234567890abcdef");
  QByteArray hash2 = QByteArray::fromHex("abcdef1234567890");

  // Create 3 files with 2 being duplciates
  FileInfo fileInfo1(filePath1, fileType, fileSize, hash1);
  FileInfo fileInfo2(filePath2, fileType, fileSize, hash2);
  FileInfo fileInfoDup(filePath1, fileType, fileSize, hash1);
  // Check FileInfo's defined operators
  QVERIFY(fileInfo1 < fileInfo2);
  QVERIFY(fileInfo1 == fileInfoDup);
}

void FileManagerTest::testFileInfoDateCreated() {
    qDebug("test date created is correct");
    fs::path fPath = fs::current_path() / "TestFiles/testfile1.txt";
    FileInfo file(fPath, QString::fromStdString(fPath.extension().string()),
                  fs::file_size(fPath),
                  testManager.HashFile("TestFiles/testfile1.txt"));
    // get date for test
    QFileInfo fileInfo(QString::fromStdString(fPath.string()));
    QDateTime time = fileInfo.birthTime();

    QCOMPARE(file.getDate(), time);
}

void FileManagerTest::testTreeWidgetHierarchy() {
  // Create tree widget
  QTreeWidget treeWidget;
  treeWidget.setColumnCount(2);

  // Add a parent item with 3 child items
  QTreeWidgetItem *parentItem = new QTreeWidgetItem(&treeWidget);
  parentItem->setText(0, "Parent Group");
  parentItem->setText(1, "0000-00-00");
  parentItem->setCheckState(0, Qt::Unchecked);

  // Add three children
  for (int i = 0; i < 3; ++i) {
    QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, "Child " + QString::number(i + 1));
    childItem->setText(1, "0000-00-00");
    childItem->setCheckState(0, Qt::Unchecked);
  }

  // Check if the parent and children added
  QCOMPARE(treeWidget.topLevelItemCount(), 1);
  QCOMPARE(parentItem->childCount(), 3);

  // Check if the child items are correctly added
  for (int i = 0; i < 3; ++i) {
    QCOMPARE(parentItem->child(i)->text(0), "Child " + QString::number(i + 1));
  }
}

void FileManagerTest::testCheckingParentMarksAllChildren() {
  // Create the main window and tree widget
  MainWindow mainWindow;
  QTreeWidget treeWidget;

  // Populate a parent item with 3 child items
  QTreeWidgetItem *parentItem = new QTreeWidgetItem(&treeWidget);
  parentItem->setText(0, "Parent Group");
  parentItem->setText(1, "0000-00-00");
  parentItem->setCheckState(0, Qt::Unchecked);
  // Add three children
  for (int i = 0; i < 3; ++i) {
    QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, "Child " + QString::number(i + 1));
    childItem->setText(1, "0000-00-00");
    childItem->setCheckState(0, Qt::Unchecked);
  }

  // Simulate the parent checkbox being checked
  parentItem->setCheckState(0, Qt::Checked);
  // call the mainwindow check logic
  mainWindow.onTreeItemChanged(parentItem);

  // Verify that all child items of the parent are checked after
  // (excluding the original file at index 0)
  for (int i = 1; i < 3; ++i) {
    QCOMPARE(parentItem->child(i)->checkState(0), Qt::Checked);
  }
}

void FileManagerTest::testUncheckingChildSetsParentPartial() {
  // Create the main window and tree widget
  MainWindow mainWindow;
  QTreeWidget treeWidget;

  // Populate parent item with 3 child items (ALL CHECKED)
  QTreeWidgetItem *parentItem = new QTreeWidgetItem(&treeWidget);
  parentItem->setCheckState(0, Qt::Checked);
  parentItem->setText(0, "Parent Group");
  parentItem->setText(1, "0000-00-00");
  // Add three children
  for (int i = 0; i < 3; ++i) {
    QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, "Child " + QString::number(i + 1));
    childItem->setText(1, "0000-00-00");
    childItem->setCheckState(0, Qt::Checked);
  }
  // Simulate one of the child checkboxes being unchecked
  // Using index 1 because index 0 is for the original file
  QTreeWidgetItem *childItem = parentItem->child(1);
  childItem->setCheckState(0, Qt::Unchecked);
  // Run logic to partially check the parent
  mainWindow.onTreeItemChanged(childItem);

  // Verify that the parent item is set to "PartiallyChecked"
  QCOMPARE(parentItem->checkState(0), Qt::PartiallyChecked);
}

void FileManagerTest::testUncheckingAllChildSetsParentUncheck() {
  // Create the main window and tree widget
  MainWindow mainWindow;
  QTreeWidget treeWidget;

  // Populate parent item with 3 child items (ALL CHECKED)
  QTreeWidgetItem *parentItem = new QTreeWidgetItem(&treeWidget);
  parentItem->setCheckState(0, Qt::Checked);
  parentItem->setText(0, "Parent Group");
  parentItem->setText(1, "0000-00-00");
  // Add three children
  for (int i = 0; i < 3; ++i) {
    QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
    childItem->setText(0, "Child " + QString::number(i + 1));
    childItem->setText(1, "0000-00-00");
    childItem->setCheckState(0, Qt::Checked);
  }
  // Simulate all of the child checkboxes being unchecked
  for (int i = 0; i < parentItem->childCount(); i++) {
    parentItem->child(i)->setCheckState(0, Qt::Unchecked);
  }
  // Run logic that should uncheck the parent
  mainWindow.onTreeItemChanged(parentItem->child(0));

  // Verify that the parent item is set to "Unchecked"
  QCOMPARE(parentItem->checkState(0), Qt::Unchecked);
}

void FileManagerTest::test_TutorialTextBrowserContent() {
    Tutorial tutorial;
    auto* textBrowser = tutorial.findChild<QTextBrowser *>("textBrowser");
    QVERIFY(textBrowser);
    QVERIFY(textBrowser->toHtml().contains("How to Use Replica Reaper"));
    QVERIFY(textBrowser->toHtml().contains("Run the Reaper"));
}

void FileManagerTest::test_TutorialExternalLinksEnabled() {
    Tutorial tutorial;
    auto* textBrowser = tutorial.findChild<QTextBrowser*>("textBrowser");
    QVERIFY(textBrowser);
    QVERIFY(textBrowser->openExternalLinks());
}

void FileManagerTest::test_TutorialButtonsCreation() {
    Tutorial tutorial;
    QStringList buttonNames = {"tryDownBTN", "tryDocBTN", "tryPicBTN", "tryDeskBTN"};
    for (const QString& btnName : buttonNames) {
        QPushButton* btn = tutorial.findChild<QPushButton*>(btnName);
        // Check if the button was created
        QVERIFY(btn);
    }
}

void FileManagerTest::test_SettingsComponentCreation() {
    // Verify all componest have been created
    Settings dialog;
    QCheckBox *backgroundCheckBox = dialog.findChild<QCheckBox*>("bgCheckBox");
    QVERIFY(backgroundCheckBox);

    QPushButton *applyButton = dialog.findChild<QPushButton*>("applyBTN");
    QVERIFY(applyButton);

    QPushButton *cancelButton = dialog.findChild<QPushButton*>("cancelBTN");
    QVERIFY(cancelButton);
}

void FileManagerTest::test_SettingsSetGetState() {
    Settings settings;
    settings.setState(true);
    QVERIFY(settings.getState() == true);

    settings.setState(false);
    QVERIFY(settings.getState() == false);
}

void FileManagerTest::test_SettingsApplySettings() {
    MainWindow mainWindow;
    Settings settings(&mainWindow);
    settings.setState(true);
    settings.applySettings();
    QVERIFY(mainWindow.getBackgroundState() == true);

    settings.setState(false);
    settings.applySettings();
    QVERIFY(mainWindow.getBackgroundState() == false);
}

void FileManagerTest::test_SettingsApplyButtonClicked() {
    MainWindow mainWindow;
    Settings settings(&mainWindow);
    settings.setState(true);
    settings.onApplyBTN_clicked();
    QVERIFY(mainWindow.getBackgroundState() == true);
    QVERIFY(settings.isHidden());
}

void FileManagerTest::test_SettingsCancelButtonClicked() {
    // Verify that settings closes when cancel button is clicked
    MainWindow window;
    Settings settings(&window);
    settings.show();
    settings.onCancelBTN_clicked();
    QVERIFY(settings.isHidden());
    QVERIFY(window.getBackgroundState() == false);
}

// Cleans up after all test functions have executed
void FileManagerTest::cleanupTestCase() {
  qDebug("Called after all test cases.");
  //  QString basePath = QCoreApplication::applicationDirPath();
  // QDir testDir(basePath);
  //  QString testFilesPath = testDir.filePath("TestFiles");

  // Remove test files created during the tests
  // QFile::remove(testFilesPath + "/testfile1.txt");
  // QFile::remove(testFilesPath + "/testfile2.txt");
  // QFile::remove(testFilesPath + "/empty.txt");
}

// this calls main for the test class
QTEST_MAIN(FileManagerTest)
// leave this inlcude alone
#include "tst_filemanager.moc"
