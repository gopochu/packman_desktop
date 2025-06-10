cmake_minimum_required(VERSION 3.16)
project(QtInstallerDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Собираем список всех исходников
set(SOURCES
    src/ApplicationCore.cpp
    src/MainWindow.cpp
    src/PackageManager.cpp
    src/PackageInfo.h
    src/PackageManager.h
    src/MainWindow.h
    resources/packages.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
