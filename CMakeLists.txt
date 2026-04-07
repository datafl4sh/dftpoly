cmake_minimum_required(VERSION 3.30)
project(polyfft)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Diskpp_DIR "/home/matteo/workingcopies/diskpp2/libdiskpp/build")
find_package(Diskpp)

find_package(Eigen3 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES_MAIN src/main.cpp src/points.cpp src/hist.cpp src/dft.cpp
    src/PolyExplorer.cpp src/PolyDFTWidget.cpp )

set(SOURCES_DISKPP src/PolygonEditorWidget_diskpp.cpp src/HHOModel.cpp
    src/StabfreeHHOParameters.cpp)

set(SOURCES ${SOURCES_MAIN})
if (Diskpp_FOUND)
    set(SOURCES ${SOURCES} ${SOURCES_DISKPP})
endif()

add_executable(polyfft ${SOURCES})

target_link_libraries(polyfft PUBLIC Qt6::Widgets Eigen3::Eigen)
if(Diskpp_FOUND)
    target_link_libraries(polyfft PUBLIC diskpp::diskpp)
    target_compile_definitions(polyfft PUBLIC "HAVE_DISKPP")
endif()
set_property(TARGET polyfft PROPERTY AUTOMOC ON)
