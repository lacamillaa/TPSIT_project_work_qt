cmake_minimum_required(VERSION 3.16)

project(qt-interface VERSION 0.1 LANGUAGES CXX)

# Abilitazioni automatiche Qt

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Standard C++

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Trova Qt (Core, GUI, Widgets, Network)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Network WebView SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Network WebView SerialPort)

# File del progetto

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    SpotifyAuthenticator.h
    SpotifyAuthenticator.cpp
    SpotifyParser.h
    SpotifyParser.cpp
    SpotifyElaborator.h
    SpotifyElaborator.cpp
)

# Creazione eseguibile

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
qt_add_executable(qt-interface
MANUAL_FINALIZATION
${PROJECT_SOURCES}
)
else()
add_executable(qt-interface
${PROJECT_SOURCES}
)
endif()

# Link librerie Qt

target_link_libraries(qt-interface PRIVATE
Qt${QT_VERSION_MAJOR}::Core
Qt${QT_VERSION_MAJOR}::Gui
Qt${QT_VERSION_MAJOR}::Widgets
Qt${QT_VERSION_MAJOR}::Network
Qt${QT_VERSION_MAJOR}::WebView
Qt${QT_VERSION_MAJOR}::SerialPort
)

# Proprietà (Windows/macOS bundle)

set_target_properties(qt-interface PROPERTIES
WIN32_EXECUTABLE TRUE
MACOSX_BUNDLE TRUE
)

# Finalizzazione Qt6

if(QT_VERSION_MAJOR EQUAL 6)
qt_finalize_executable(qt-interface)
endif()
