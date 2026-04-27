cmake_minimum_required(VERSION 3.22)
project(DebenGui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(DebenGui MANUAL_FINALIZATION
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GUImain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindow.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindow.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/DebenController.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/DebenController.h"
)

target_link_libraries(DebenGui PRIVATE Qt6::Widgets)

set_target_properties(DebenGui PROPERTIES WIN32_EXECUTABLE FALSE)

qt_finalize_executable(DebenGui)