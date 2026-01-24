cmake_minimum_required(VERSION 3.21)
project(sniffles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Charts Widgets)

qt_standard_project_setup()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

qt_add_executable(sniffles
    main.cpp
)

qt_add_qml_module(sniffles
    URI sniffles
    VERSION 1.0
    QML_FILES
        main.qml
)

target_link_libraries(sniffles
    PRIVATE
        Qt6::Quick
        Qt6::Charts
        Qt6::Widgets
)

