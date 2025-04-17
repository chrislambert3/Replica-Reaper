cmake_minimum_required(VERSION 3.16)

project(Replica-Reaper VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.hpp
        mainwindow.ui
)

# Add trashmanager.hpp so it's visible in all cases
list(APPEND PROJECT_SOURCES trashmanager.hpp)

# Add platform-specific TrashManager definition
# This will add the right cpp file to the project source
# and makes the other file not compile and appear under "Cross Platform Files"
# HEADER_FILE_ONLY TRUE = treat the file as a .hpp header (not compiled)
if(WIN32)
    list(APPEND PROJECT_SOURCES trashmanager_win.cpp)
    set_source_files_properties(trashmanager_mac.mm PROPERTIES HEADER_FILE_ONLY TRUE)
    source_group("Cross Platform Files" FILES trashmanager_mac.mm)
elseif(APPLE)
    list(APPEND PROJECT_SOURCES trashmanager_mac.mm)
    set_source_files_properties(trashmanager_mac.mm PROPERTIES COMPILE_FLAGS "-x objective-c++")
    set_source_files_properties(trashmanager_win.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
    source_group("Cross Platform Files" FILES trashmanager_win.cpp)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Replica-Reaper
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        filemanager.hpp filemanager.cpp
        FileInfo.hpp
        settings.hpp settings.cpp settings.ui
        tutorial.hpp tutorial.cpp tutorial.ui
        assets.qrc
        trashmanager.hpp trashmanager_win.cpp trashmanager_mac.mm
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Replica-Reaper APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Replica-Reaper SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Replica-Reaper
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Logic to determine if move to trash feature is implemented
# Currently only windows has it
if(WIN32)
    target_compile_definitions(Replica-Reaper PRIVATE TRASH_AVAILABLE)
elseif(APPLE)
    # if mac gets implemented copy line here too
endif()

target_link_libraries(Replica-Reaper PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Replica-Reaper)
endif()
set_target_properties(Replica-Reaper PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Replica-Reaper
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Replica-Reaper)
endif()
add_subdirectory(UnitTest)
