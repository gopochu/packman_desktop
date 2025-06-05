cmake_minimum_required(VERSION 3.20)

project(pacman LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

if(NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt5 not found. Please install Qt5 development packages.")
endif()



set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20
)
add_executable(pacman 
    src/ApplicationCore.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/PackageManager.cpp
    src/PackageManager.h
    src/PackageInfo.h
)

target_link_libraries(pacman PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)