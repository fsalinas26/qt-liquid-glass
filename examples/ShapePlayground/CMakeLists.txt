cmake_minimum_required(VERSION 3.16)

project(ShapePlayground LANGUAGES CXX)

set(CMAKE_AUTORCC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Widgets)

add_executable(ShapePlayground
    main.cpp
    ShapePlayground.cpp
    ShapePlayground.h
    ../GlassDemo/resources.qrc
)

target_link_libraries(ShapePlayground PRIVATE
    Qt6::Core
    Qt6::Widgets
    QtLiquidGlass
)
