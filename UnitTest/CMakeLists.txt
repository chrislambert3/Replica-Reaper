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


set(PLATFORM_SOURCES
    ../filemanager.cpp
    ../mainwindow.cpp
    ../settings.cpp
    ../tutorial.cpp
    tst_filemanager.cpp
)
 # determine which file belongs in unit test sources
if(WIN32)
    list(APPEND PLATFORM_SOURCES ../trashmanager_win.cpp)
elseif(APPLE)
    list(APPEND PLATFORM_SOURCES ../trashmanager_mac.mm)
endif()

# Create the unit test executable
add_executable(UnitTests ${PLATFORM_SOURCES})
target_link_libraries(UnitTests PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Widgets)

#<--- This line implements the whole file as ONE test case for Ctest--->
add_test(NAME UnitTests COMMAND UnitTests)
