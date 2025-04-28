cmake_minimum_required(VERSION 3.16)

project(UnitTests LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Gui Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Test)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#***** Intructions to add a new file in CMake config:
# 1. Right Click "Source Files" -> Add New -> Select: SOURCE FILE (NOT A C++ Class or youll get a .h file)
# 2. Name it tst_<whatever>.cpp
# 3. add another add executable line (below)
# Ex. If <somefile>.cpp has " #include ../<someheader>.h "
# add_executable(UnitTests ... ... "tst_<somefile>.cpp"  "../<whatever was inluded mathcing cpp file>.cpp" )
add_executable(UnitTests tst_filemanager.cpp ../filemanager.cpp ../mainwindow.cpp ../settings.cpp ../tutorial.cpp
    assets.qrc)
target_link_libraries(UnitTests PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Widgets)

#<--- This line implements the whole file as ONE test case for Ctest--->
add_test(NAME UnitTests COMMAND UnitTests)
