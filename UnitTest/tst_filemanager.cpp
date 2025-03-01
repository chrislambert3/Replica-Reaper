#include <QCoreApplication>
#include <QTest>
#include "../filemanager.h"
#include <QTemporaryFile>
// add necessary includes here

/*
Built in function calls:
initTestCase() will be called before the first test function is executed.
<insert testnamefunction>_data() will be called to create a global test data table.
cleanupTestCase() will be called after the last test function was executed.
init() will be called before each test function is executed.
cleanup() will be called after every test function.
 */

class FileManagerTest : public QObject
{
    Q_OBJECT
public:
    explicit FileManagerTest(QObject *parent = nullptr);
    ~FileManagerTest();

private slots:
    // creates a directory "TestFiles" as well as 3 files in the direcotry
    void initTestCase(); // initTestCase() will be called before any test function is executed.

    //<testnamefunction>_data() will be called to create a global test data table.
    // Ex. testHashFile_data() will get called inside testHashFile()
    void testHashFile();
    void testHashFile_data();
    void testHashFile_FileNotFound();

    void myFirstTest();  // sample test with conditionals
    void mySecondTest(); // another sample

    void cleanupTestCase(); // cleanupTestCase() will be called after the last test function was executed.
    // *****currently commented instructions to delete the 3 files from initTestCase()

private:
    FileManager testManager;
};
FileManagerTest::FileManagerTest(QObject *parent):QObject(parent){}
FileManagerTest::~FileManagerTest() {}


void FileManagerTest::initTestCase() // will be called before the first test case is executed.
{

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

    QFile emptyFile(testFilesPath + "/empty.txt");
    emptyFile.open(QIODevice::WriteOnly);
    emptyFile.close();
}
// This creates a data table scoped to only testHashFile() test function
// Since theres 3 Rows testHashFile will run 3 times
void FileManagerTest::testHashFile_data()
{
    QTest::addColumn<QString>("filePath");
    QTest::addColumn<QByteArray>("expectedHash");

    QString basePath = QCoreApplication::applicationDirPath();
    QDir testDir(basePath);
    QString testFilesPath = testDir.filePath("TestFiles");

    // Add test data for files and their expected hash values
    // *newRow(" custon test case name") << ** "filePath" column entry ** << ** "expected
    QTest::newRow("Testing File Hash: test file 1") << testFilesPath + "/testfile1.txt" << QByteArray("dffd6021bb2bd5b0af676290809ec3a53191dd81c7f70a4b28688a362182986f");
    QTest::newRow("Testing File Hash: test file 2") << testFilesPath + "/testfile2.txt" << QByteArray("564b97c72b1c73c308baecea3cece413e8026bfd76cd902fd2a0ea9e3febd0b2");
    QTest::newRow("Testing File Hash: empty file")  << testFilesPath + "/empty.txt"     << QByteArray("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    qDebug() << "Hash file data table created";
}
// Test function that compares file hashes
void FileManagerTest::testHashFile()
{
    QFETCH(QString, filePath);
    QFETCH(QByteArray, expectedHash);

    QByteArray actualHash = testManager.HashFile(filePath);  // Call the method to get the hash
    // qDebug() << "Path: " <<filePath;
    // qDebug() << "Hash: " << actualHash;
    QCOMPARE(actualHash, expectedHash);  // Compare actual hash with expected hash
}
// this shoudl trigger
void FileManagerTest::testHashFile_FileNotFound(){
    QTest::ignoreMessage(QtWarningMsg, "Could not open file: \"non_existent_file.txt\"");
    QByteArray result = testManager.HashFile("non_existent_file.txt");
    QVERIFY(result.isEmpty());
}

void FileManagerTest::myFirstTest(){
    QVERIFY(true); // check that a condition is satisfied
    QCOMPARE(1, 1); // compare two value
}
void FileManagerTest::mySecondTest(){
    QVERIFY(true);
    QVERIFY(1 != 2); // check conditional
    QCOMPARE(1, 1); // comapare values

    // Additional QCOMPARE examples
    int a = 5;
    int b = 5;
    QCOMPARE(a, b); // Passes if a == b

    QString str1 = "Hello";
    QString str2 = "Hello";
    QCOMPARE(str1, str2); // Passes if str1 == str2
    // Additional QVERIFY example
    QVERIFY(10 > 5); // Passes if the condition is true
}



// Cleans up after all test functions have executed
void FileManagerTest::cleanupTestCase()
{
    qDebug("Called after all test cases.");
    //  QString basePath = QCoreApplication::applicationDirPath();
    // QDir testDir(basePath);
    //  QString testFilesPath = testDir.filePath("TestFiles");

    // Remove test files created during the tests
    // QFile::remove(testFilesPath + "/testfile1.txt");
    // QFile::remove(testFilesPath + "/testfile2.txt");
    // QFile::remove(testFilesPath + "/empty.txt");
}

QTEST_MAIN(FileManagerTest)

#include "tst_filemanager.moc"
