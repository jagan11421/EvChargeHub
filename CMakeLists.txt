
cmake_minimum_required(VERSION 3.16)

project(EVChargeHub VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_executable(EVChargeHub
    src/main.cpp

    src/ui/MainWindow.h
    src/ui/MainWindow.cpp

    src/ui/pages/DashboardPage.h
    src/ui/pages/DashboardPage.cpp

    src/ui/pages/PlaceholderPage.h
    src/ui/pages/PlaceholderPage.cpp
)

target_include_directories(EVChargeHub PRIVATE src)

target_link_libraries(EVChargeHub PRIVATE
    Qt6::Widgets
    Qt6::Sql
)
