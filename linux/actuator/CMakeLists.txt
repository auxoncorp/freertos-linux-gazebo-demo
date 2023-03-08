cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

project(actuator LANGUAGES CXX)

find_package(gz-cmake3 REQUIRED)
find_package(gz-msgs9 REQUIRED)
find_package(gz-transport12 REQUIRED)

add_library(actuator SHARED actuator.cpp)

set_property(TARGET actuator PROPERTY CXX_STANDARD 17)

target_include_directories(
    actuator
    PUBLIC
    include)

target_link_libraries(
    actuator
    PRIVATE
    gz-msgs9::gz-msgs9
    gz-transport12::gz-transport12)

set_target_properties(actuator PROPERTIES PUBLIC_HEADER "include/actuator/actuator.h")

install(
    TARGETS
    actuator
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
    PUBLIC_HEADER DESTINATION include/actuator
)
