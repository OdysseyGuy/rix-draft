cmake_minimum_required(VERSION 3.15)
message(STATUS "CMake version: ${CMAKE_VERSION}")
project(rix LANGUAGES C CXX)

# minimal compiler version check
macro(rix_minimal_compiler_version)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "{ARGV0}" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "{ARGV1}")
        message(FATAL_ERROR "Compiler version is too low: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} requires at least ${ARGV1}")
    endif()
endmacro()

rix_minimal_compiler_version("GNU", 10.0)
rix_minimal_compiler_version("Clang", 12.0)

# generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -mno-red-zone -mcmodel=large -fno-stack-protector -fno-builtin")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DKERNEL_VMA_BASE=0xffffffff80000000")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DKERNEL_ASPACE_BASE=0xffffff8000000000")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DKERNEL_ASPACE_SIZE=0x0000008000000000")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -gdwarf-2 -O0")

set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -n -nostdlib -mno-red-zone -flto")

