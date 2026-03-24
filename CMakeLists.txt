cmake_minimum_required(VERSION 3.16)
project(polyfft)

find_package(Eigen3 REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(polyfft
    src/main.cpp
    src/points.cpp src/hist.cpp src/dft.cpp
    src/PolyExplorer.cpp src/PolyDFTWidget.cpp
    src/StabfreeHHOParameters.cpp
)

target_link_libraries(polyfft Qt6::Widgets Eigen3::Eigen)
set_property(TARGET polyfft PROPERTY AUTOMOC ON)
