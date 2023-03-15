cmake_minimum_required(VERSION 3.5)

project(actuator-app LANGUAGES C)

if(NOT CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 99)
endif()

add_executable(
    ${PROJECT_NAME}
    src/tp.c
    src/main.c)

target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE
    _GNU_SOURCE)

target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    -Wall -Wextra -Werror
    -Wshadow -Wmissing-include-dirs
    -Wno-unused-parameter -Wno-pointer-to-int-cast
    -Wno-sign-compare)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    include)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    dl lttng-ust actuator)

target_link_directories(
    ${PROJECT_NAME}
    PUBLIC
    /usr/local/lib)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    INSTALL_RPATH
    "/usr/local/lib"
)

install(
    TARGETS
    ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
