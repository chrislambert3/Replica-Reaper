# REPLICA REAPER

<details>
<summary>Table of Contents</summary>

- [REPLICA REAPER](#replica-reaper)
  - [Overview](#overview)
  - [Preconditions](#preconditions)
  - [How to Build](#how-to-build)
  - [How to Test](#how-to-test)
  - [Contact Info](#contact-info)

</details>

## Overview

Currently, checking your computer for duplicate files without the help of third-party software is
tedious and inefficient. Users are required to manually open their file manager, start a search
for files with names that include "(1)" or "(2)", and filter through an overwhelming amount of
unrelated files. Even once a duplicate file is identified, the user must manually move this file
to their recycling/trash bin on their device. This is a time consuming task that could take days
depending on the size and clutter of the file system.
Replica Reaper eliminates the tedious and inefficiency of this by completely automating the
entire process. It detects and removes duplicate files in a timely manner, efficiently. Unlike many
other existing solutions that simply match filenames, or advanced duplicate detection being locked
behind paywalls, Replica Reaper provides a robust and accessible solution for users. Beyond
the basic file system cleanup feature, the software will also prevent users from unintentionally
downloading duplicate files, creating a solution that will solve the problem for good.

## Preconditions

- Windows OS
- [QtCreator](https://www.qt.io/download-qt-installer-oss) Installation

## How to Build

### Build In Qt Creator:

1. Pull the source code and open the project in QtCreator by selecting the corresponding `CMakeLists.txt` file in the directory
2. Click the hammer icon on the bottom left cornor of of UI to build
3. Click the green play button to run the program

### Build from the Command Line:

1. To build this project you need to use **CMake** and **Ninja** build systems found in your QT installation directory
2. Assuming you have the default installation path, open Command Prompt and load the build environment file located at `C:\Qt\<QT-VERSION>\mingw_64\bin\qtenv2.bat`. Replace `<QT-VERSION>` with whatever version number is in your installation. This project was built with version `6.8.2`

3. WIth the same environment run:

```sh
cmake -G Ninja -S C:/path/to/<source-dir> -B C:/path/to/<build-dir> "-DCMAKE_BUILD_TYPE:STRING=Release"
```

Replace `<source-dir>` with the path to your source code and `<build-dir>` with the desired build output directory.<br>
Optional: Change `DCMAKE_BUILD_TYPE:STRING=Release` to "Debug" if you want debug statements 4. Change into the `<build-dir>` directory and run:

```sh
ninja
```

## How to Deploy

1. To package the executable and its dependencies into one directory copy only the `Replica-Reaper.exe` executable into an empty folder.
2. In the Command Prompt run:

```sh
C:\Qt\<QT-VERSION>\mingw_64\bin\windeployqt C:\path\to\folder
```

4. In the folder should now have all the dependencies needed to deploy and run on other windows machines

## How to Build the Installer

1. To build the installer you must install the additional QT Installer Framework which can be installers from QT Creator's Maintenace tool.
2. From the IDE, click on the Tools tab at the top, select maintenace tool, and install the framework found under QT→Build Tools→QT Installer Framework
3. You should have an executable located at `C:\Qt\Tools\QtInstallerFramework\<QT-VERSION>\bin\binarycreator.exe`
4. Follow the Instrictions from [How to Deploy](#how-to-deploy) but with the dependencies packaged in `Installer\packages\com.swe.replicareaper\data`
5. Run the Installer with the following parameters for configuration, packages, and executable name:

```sh
binarycreator.exe -c Installer/config/config.xml -p RR-Build/Installer/packages RR-win64-Installer
```

## How to Test

### Test in QT Creator

1. With the project open in QtCreator, make sure you are in `Debug` mode
2. Select the tool icon in the bottom left corner under `Debug` to build the project
3. Under the `Tools` dropdown at the top of your screen, navigate to `Tests` and select `Run All Tests`
4. View Results in the Test Window

### Test from the Command Line:

1. Follow instructions from [How to Deploy](#how-to-deploy) but with the `UnitTests.exe` found in the `UnitTest` subdirectory
2. Run `UnitTests.exe` from the command prompt
3. Results will print to the console

### Test run time using Auto.py/generate table

1. Clone our main Github Repository
2. Have the qtCreator IDE installed locally
3. Open the project in qtCreator using Cmakelists.txt
4. Confirm that "#define DEBUG" is set to 0 in main.cpp
5. Build and run the program, this will test run time
   on various generic folders
6. Navigate to "Auto.py" in the repositories main folder
7. run "pip install cpuinfo" in the terminal if this library
   not installed locally.
8. Run "Auto.py" with your preferred interpreter.

## Contact Info

- Braden Maillet: Graphics Engineer

  - Email: braden_maillet@student.uml.edu
  - [LinkedIn](https://www.linkedin.com)
  - ![image](/images/team_members/)

- Evan Casasanta: Project Manager

  - Email: evan_casasanta@student.uml.edu
  - [LinkedIn](https://www.linkedin.com)
  - ![image](/images/team_members/)

- Chris Lambert: Software Engineer

  - Email: christopher_lambert@student.uml.edu
  - [LinkedIn](https://www.linkedin.com)
  - ![image](/images/team_members/)

- Gavin Ippolito: Documentation Lead

  - Email: gavin_ippolito@student.uml.edu
  - [LinkedIn](https://www.linkedin.com)
  - ![image](/images/team_members/)

- Isaiah Andrade: Test Developer
  - Email: isaiah_andrade@student.uml.edu
  - [LinkedIn](https://www.linkedin.com/in/isaiah-andrade/)
  - ![Isaiah Profile](/images/team_members/isaiah_andrade.jpg)
