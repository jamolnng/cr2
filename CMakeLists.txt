cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
cmake_policy(VERSION 3.10)

if(UNIX)
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_EXECUTABLE_SUFFIX "")
elseif(WIN32)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_EXECUTABLE_SUFFIX ".exe")
endif()

set(bsp "${CMAKE_SOURCE_DIR}/bsp/hifive1b")

set(tools_dir "${CMAKE_SOURCE_DIR}/tools/${CMAKE_SYSTEM_NAME}")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

set(CMAKE_C_COMPILER "${tools_dir}/riscv64-unknown-elf-gcc/bin/riscv64-unknown-elf-gcc${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_CXX_COMPILER "${tools_dir}/riscv64-unknown-elf-gcc/bin/riscv64-unknown-elf-g++${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_ASM_COMPILER "${tools_dir}/riscv64-unknown-elf-gcc/bin/riscv64-unknown-elf-gcc${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_LINKER "${tools_dir}/riscv64-unknown-elf-gcc/bin/riscv64-unknown-elf-ld${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

project(cs2 VERSION 0.0.1 LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-Os")
set(CMAKE_ASM_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_ASM_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -nostdlib -nostartfiles -nodefaultlibs -march=rv32imac -mabi=ilp32 -mcmodel=medany \
    -pedantic -pedantic-errors -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization \
    -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs \
    -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=5 -Wswitch-default -Wundef \
    -Werror -Wno-unused -Winline -Wconversion -Wfloat-equal -ffreestanding -fdata-sections \
    -ffunction-sections -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${CMAKE_C_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ffunction-sections -Wl,--gc-sections -nostartfiles -nodefaultlibs -nostdlib -T \"${bsp}/linker.ld\"")

file(GLOB_RECURSE SOURCES "src/*.c" "src/*.S" "${bsp}/src/*.c" "${bsp}/src/*.S" "libc/src/*.c" "libc/src/*.S")
file(GLOB_RECURSE PUBLIC_HEADERS "include/*.h" "${bsp}/include/*.h" "libc/include/*.h")
file(GLOB_RECURSE PRIVATE_HEADERS "src/*.h")

set_source_files_properties("src/cr2.S" PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp")

add_executable(cr2 ${SOURCES} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})
target_include_directories(cr2 PRIVATE src "${bsp}/src")
target_include_directories(cr2 PUBLIC include "${bsp}/include" "libc/include")
set_target_properties(cr2 PROPERTIES LINK_DEPENDS "${bsp}/linker.ld")

add_custom_target(cr2hex
    ALL
    BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/cr2.hex"
    COMMAND ${tools_dir}/riscv64-unknown-elf-gcc/bin/riscv64-unknown-elf-objcopy${CMAKE_EXECUTABLE_SUFFIX} -S -O ihex $<TARGET_FILE:cr2> "${CMAKE_CURRENT_BINARY_DIR}/cr2.hex"
)

add_custom_target(cr2asm
    ALL
    BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/cr2.hex"
    COMMAND ${tools_dir}/riscv64-unknown-elf-gcc/bin/riscv64-unknown-elf-objdump${CMAKE_EXECUTABLE_SUFFIX} -dS $<TARGET_FILE:cr2> > "${CMAKE_CURRENT_BINARY_DIR}/cr2.S"
)

set_target_properties(cr2hex PROPERTIES ADDITIONAL_CLEAN_FILES "cr2.hex")
set_target_properties(cr2asm PROPERTIES ADDITIONAL_CLEAN_FILES "cr2.S")

add_dependencies(cr2hex cr2)
add_dependencies(cr2asm cr2)