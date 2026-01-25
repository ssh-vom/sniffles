cmake_minimum_required(VERSION 3.21)
project(sniffles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Charts Widgets)

qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)

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

find_path(PCAPPLUSPLUS_INCLUDE_DIR
    NAMES IPv4Layer.h
    PATHS /opt/homebrew/opt/pcapplusplus/include/pcapplusplus /opt/homebrew/include/pcapplusplus /usr/local/include/pcapplusplus
)

find_library(PCAPPLUSPLUS_LIB
    NAMES Pcap++ PcapPlusPlus
    PATHS /opt/homebrew/opt/pcapplusplus/lib /opt/homebrew/lib /usr/local/lib
)

find_library(PACKETPP_LIB
    NAMES Packet++
    PATHS /opt/homebrew/opt/pcapplusplus/lib /opt/homebrew/lib /usr/local/lib
)

find_library(COMMONPP_LIB
    NAMES Common++
    PATHS /opt/homebrew/opt/pcapplusplus/lib /opt/homebrew/lib /usr/local/lib
)

find_library(PCAP_LIB
    NAMES pcap
    PATHS /opt/homebrew/lib /usr/local/lib
)

if (NOT PCAPPLUSPLUS_INCLUDE_DIR OR NOT PCAPPLUSPLUS_LIB OR NOT PACKETPP_LIB OR NOT COMMONPP_LIB OR NOT PCAP_LIB)
    message(FATAL_ERROR "PcapPlusPlus/libpcap not found. Install with brew install pcapplusplus.")
endif()

add_executable(capture
    src/capture.cpp
)

target_include_directories(capture
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PCAPPLUSPLUS_INCLUDE_DIR}
)

target_link_libraries(capture
    PRIVATE
        ${PCAPPLUSPLUS_LIB}
        ${PACKETPP_LIB}
        ${COMMONPP_LIB}
        ${PCAP_LIB}
)

target_include_directories(sniffles
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(sniffles
    PRIVATE
        Qt6::Quick
        Qt6::Charts
        Qt6::Widgets
)

