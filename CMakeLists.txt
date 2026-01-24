cmake_minimum_required(VERSION 3.21)
project(MyApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE Qt6::Widgets)

