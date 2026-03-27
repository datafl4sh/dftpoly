cmake_minimum_required(VERSION 3.30)
project(polyfft)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Diskpp_DIR "/home/matteo/workingcopies/diskpp2/libdiskpp/build")
find_package(Diskpp)

message(STATUS "MP: ${CMAKE_MODULE_PATH}")

find_package(Eigen3 REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(polyfft
    src/main.cpp
    src/points.cpp src/hist.cpp src/dft.cpp
    src/PolyExplorer.cpp src/PolyDFTWidget.cpp
    src/StabfreeHHOParameters.cpp
)

target_link_libraries(polyfft Qt6::Widgets Eigen3::Eigen diskpp::diskpp)
set_property(TARGET polyfft PROPERTY AUTOMOC ON)
