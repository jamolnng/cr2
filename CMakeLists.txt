cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

project(cs2 VERSION 0.0.1 LANGUAGES C ASM)

set(CMAKE_C_COMPILER "${CMAKE_SOURCE_DIR}/tools/riscv64-unknown-elf-gcc/bin/riscv64-unknown-elf-gcc")
set(CMAKE_CXX_COMPILER "${CMAKE_SOURCE_DIR}/tools/riscv64-unknown-elf-gcc/bin/riscv64-unknown-elf-g++")
set(CMAKE_ASM_COMPILER "${CMAKE_SOURCE_DIR}/tools/riscv64-unknown-elf-gcc/bin/riscv64-unknown-elf-as")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=rv32imac -mabi=ilp32")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -nostdlib -nostartfiles -nodefaultlibs -g -march=rv32imac -mabi=ilp32 -mcmodel=medany \
    -pedantic -pedantic-errors -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization \
    -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs \
    -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=5 -Wswitch-default -Wundef \
    -Werror -Wno-unused -Winline -Wconversion -Wfloat-equal")

set(bsp "${CMAKE_SOURCE_DIR}/bsp/hifive1b")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T \"${bsp}/linker.ld\"")

file(GLOB_RECURSE SOURCES "src/*.c" "src/*.asm" "${bsp}/src/*.c" "libc/src/*.c")
file(GLOB_RECURSE PUBLIC_HEADERS "include/*.h" "${bsp}/include/*.h" "libc/include/*.h")
file(GLOB_RECURSE PRIVATE_HEADERS "src/*.h")

function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

assign_source_group(${SOURCES})
assign_source_group(${PUBLIC_HEADERS})
assign_source_group(${PRIVATE_HEADERS})

add_executable(cr2 ${SOURCES} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})
target_include_directories(cr2 PRIVATE src "${bsp}/src")
target_include_directories(cr2 PUBLIC include "${bsp}/include" "libc/include")
set_target_properties(${TARGET_NAME} PROPERTIES LINK_DEPENDS "${bsp}/linker.ld")

add_custom_command(TARGET cr2 POST_BUILD
    COMMAND ${CMAKE_SOURCE_DIR}/tools/riscv64-unknown-elf-gcc/bin/riscv64-unknown-elf-objcopy -O ihex $<TARGET_FILE:cr2> $<TARGET_FILE:cr2>.hex
)