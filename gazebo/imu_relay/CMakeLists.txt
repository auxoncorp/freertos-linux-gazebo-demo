cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

project(ImuRelay)

include(FetchContent)

find_package(gz-cmake3 REQUIRED)

find_package(gz-plugin2 REQUIRED COMPONENTS register)
set(GZ_PLUGIN_VER ${gz-plugin2_VERSION_MAJOR})

find_package(gz-sim7 REQUIRED)

FetchContent_Declare(
    frozen
    GIT_REPOSITORY "https://github.com/cesanta/frozen.git"
    GIT_TAG "1.6"
)

FetchContent_MakeAvailable(frozen)

add_library(ImuRelay SHARED ImuRelay.cc ${frozen_SOURCE_DIR}/frozen.c)

set_property(TARGET ImuRelay PROPERTY CXX_STANDARD 17)

target_include_directories(ImuRelay
    PRIVATE
    ${frozen_SOURCE_DIR})

target_link_libraries(ImuRelay
    PRIVATE
    gz-plugin${GZ_PLUGIN_VER}::gz-plugin${GZ_PLUGIN_VER}
    gz-sim7::gz-sim7)

add_custom_target(
    run-sim
    DEPENDS ImuRelay)

add_custom_command(
    TARGET run-sim
    POST_BUILD
    COMMAND
    GZ_SIM_SYSTEM_PLUGIN_PATH=${CMAKE_BINARY_DIR} gz sim ${CMAKE_SOURCE_DIR}/world.sdf --verbose 3
    COMMENT "Starting the example world")
