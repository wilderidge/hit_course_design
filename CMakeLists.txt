# CMake Project settings
cmake_minimum_required (VERSION 3.8)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
project("task")
set(PROJECT_NAME "task_")

# C Compiler settings
add_compile_options(-Werror -g)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_BUILD_TYPE Debug)                             # Type : Debug
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)   # Bin Path : bin

# Compile and build
include_directories(
    ${PROJECT_SOURCE_DIR}/public
    ${PROJECT_SOURCE_DIR}
)
aux_source_directory(${PROJECT_SOURCE_DIR}/public SRC_LIST)

add_executable (${PROJECT_NAME}1 task_1.c ${SRC_LIST})
target_link_libraries(${PROJECT_NAME}1 pthread)

add_executable (${PROJECT_NAME}2 task_2.c ${SRC_LIST})
target_link_libraries(${PROJECT_NAME}2 pthread)

add_executable (${PROJECT_NAME}3 task_3.c ${SRC_LIST})
target_link_libraries(${PROJECT_NAME}3 pthread)

# add_subdirectory(${PROJECT_SOURCE_DIR}/test)