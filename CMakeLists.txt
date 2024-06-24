# CMake Project settings
cmake_minimum_required (VERSION 3.8)
set(CMAKE_C_STANDARD 11)
project("task")

# C Compiler settings
add_compile_options(-Werror -g)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_BUILD_TYPE Debug)                             # Type : Debug
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)   # Bin Path : bin

# Compile and build
include_directories(
    ${PROJECT_SOURCE_DIR}/public
)
aux_source_directory(${PROJECT_SOURCE_DIR}/public SRC_LIST)

set(PROJECT_NAME "task_1")
add_executable (${PROJECT_NAME} task_1.c ${SRC_LIST})

set(PROJECT_NAME "task_2")
add_executable (${PROJECT_NAME} task_2.c ${SRC_LIST})