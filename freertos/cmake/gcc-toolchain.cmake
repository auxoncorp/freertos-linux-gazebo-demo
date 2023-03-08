set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)

# Assume the toolchain is setup correctly
set(CMAKE_C_COMPILER_WORKS TRUE CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER_WORKS TRUE CACHE INTERNAL "")
set(CMAKE_C_ABI_COMPILED TRUE CACHE INTERNAL "")
set(CMAKE_CXX_ABI_COMPILED TRUE CACHE INTERNAL "")

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Programs from the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# Libraries and includes from the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(BUILD_SHARED_LIBS OFF)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR         arm-none-eabi-ar)
set(CMAKE_LINKER     arm-none-eabi-ld)
set(CMAKE_NM         arm-none-eabi-nm)
set(CMAKE_OBJDUMP    arm-none-eabi-objdump)
set(CMAKE_OBJCOPY    arm-none-eabi-objcopy)
set(CMAKE_STRIP      arm-none-eabi-strip)
set(CMAKE_RANLIB     arm-none-eabi-gcc-ranlib)
set(CMAKE_SIZE       arm-none-eabi-size)

set(MCU_CFLAGS "-mcpu=cortex-m4 -mlittle-endian -mthumb -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant")
set(COMMON_CFLAGS "${MCU_CFLAGS} -ffunction-sections -fdata-sections -Wall -Wextra -Werror -Wshadow -Wmissing-include-dirs -Wno-unused-parameter -Wno-pointer-to-int-cast -Wno-sign-compare")
# Assume that PROJECT_NAME == executable name for consistency
set(
    COMMON_LFLAGS
    "-T${CMAKE_SOURCE_DIR}/linker/STM32F407VGTx_FLASH.ld -Wl,--gc-sections -Xlinker -Map=${PROJECT_NAME}.map -specs=nosys.specs")

set(CMAKE_C_FLAGS "" CACHE STRING "C compiler flags")
set(CMAKE_C_FLAGS_DEBUG "" CACHE STRING "C compiler Debug flags")
set(CMAKE_C_FLAGS_RELEASE "" CACHE STRING "C compiler Release flags")
set(CMAKE_C_FLAGS_MINSIZEREL "" CACHE STRING "C compiler MinSizeRel flags")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "" CACHE STRING "C compiler RelWithDebInfo flags")
set(CMAKE_EXE_LINKER_FLAGS "" CACHE STRING "Executable linker flags")

set(CMAKE_C_FLAGS "${COMMON_CFLAGS}")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_EXE_LINKER_FLAGS "${COMMON_LFLAGS}")

if(NOT CMAKE_BUILD_TYPE)
    message(WARNING "No CMAKE_BUILD_TYPE value specified, defaulting to RelWithDebInfo.")
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Debug|Release|RelWithDebInfo|MinSizeRel" FORCE)
else()
    message(STATUS "The CMake build type is: ${CMAKE_BUILD_TYPE}")
endif()

# Display size info for an ELF file target
# Assume that PROJECT_NAME == executable name for consistency
function(print_exec_size)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_SIZE} ${PROJECT_NAME})
endfunction(print_exec_size)

# Generates a binary file from ELF
# Assume that PROJECT_NAME == executable name for consistency
function(elf_to_bin)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin
        BYPRODUCTS ${PROJECT_NAME}.hex
        COMMENT "Converting ${PROJECT_NAME} to ${PROJECT_NAME}.bin")
endfunction(elf_to_bin)
