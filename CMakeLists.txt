cmake_minimum_required(VERSION 3.21)
project(sniffles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

find_path(PCAPPLUSPLUS_INCLUDE_DIR
    NAMES PcapLiveDeviceList.h
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
    message(FATAL_ERROR "PcapPlusPlus/libpcap not found. Install with: brew install pcapplusplus libpcap")
endif()

set(SNIFFLES_CORE_SOURCES
    src/app/App.cpp
    src/capture/DeviceEnumerator.cpp
    src/capture/CaptureService.cpp
    src/capture/BpfFilter.cpp
    src/decode/Decoders.cpp
    src/decode/FlowTracker.cpp
    src/ui/MainWindow.cpp
    src/ui/models/PacketTableModel.cpp
    src/util/Logging.cpp
    src/util/Time.cpp
)

add_library(sniffles_core STATIC ${SNIFFLES_CORE_SOURCES})

target_include_directories(sniffles_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${PCAPPLUSPLUS_INCLUDE_DIR}
)

target_link_libraries(sniffles_core
    PUBLIC
        Qt6::Core
        ${PCAPPLUSPLUS_LIB}
        ${PACKETPP_LIB}
        ${COMMONPP_LIB}
        ${PCAP_LIB}
)

if (APPLE)
    target_link_libraries(sniffles_core
        PUBLIC
            "-framework CoreFoundation"
            "-framework SystemConfiguration"
    )
endif()

add_executable(sniffles
    src/main.cpp
)

target_link_libraries(sniffles
    PRIVATE
        sniffles_core
)

enable_testing()

add_executable(test_decode
    tests/test_decode.cpp
)

target_link_libraries(test_decode
    PRIVATE
        sniffles_core
)

add_test(NAME test_decode COMMAND test_decode)
