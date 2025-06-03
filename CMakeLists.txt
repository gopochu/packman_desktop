cmake_minimum_required(VERSION 3.20)

project(pacman LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(fmt REQUIRED)

if(NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt5 not found. Please install Qt5 development packages.")
endif()
if(NOT fmt_FOUND)
    message(FATAL_ERROR "fmt not found. Please install fmt library.")
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
)

target_link_libraries(pacman PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    fmt::fmt
)