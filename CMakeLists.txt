# CMake Project settings
cmake_minimum_required (VERSION 3.8)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
project("task")
set(PROJECT_NAME "task_")
find_package(OpenMP REQUIRED)

if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# C Compiler settings
add_compile_options(-Werror -g -mavx2)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_BUILD_TYPE Debug)                             # Type : Debug
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)   # Bin Path : bin

# Compile and build
include_directories(
    ${PROJECT_SOURCE_DIR}/public
    ${PROJECT_SOURCE_DIR}
)
aux_source_directory(${PROJECT_SOURCE_DIR}/public SRC_LIST)

#创建静态库
add_library(check_matrix 
    task_1.c
)

add_executable (${PROJECT_NAME}1 task_1.c ${SRC_LIST})
target_link_libraries(${PROJECT_NAME}1 pthread)

add_executable (${PROJECT_NAME}2 task_2.c ${SRC_LIST})
target_link_libraries(${PROJECT_NAME}2 PUBLIC OpenMP::OpenMP_C)

add_executable (${PROJECT_NAME}3 task_3.c ${SRC_LIST})
target_link_libraries(${PROJECT_NAME}3 PUBLIC OpenMP::OpenMP_C check_matrix)

add_executable (${PROJECT_NAME}avx task_avx.c ${SRC_LIST})
target_link_libraries(${PROJECT_NAME}avx pthread)

# add_subdirectory(${PROJECT_SOURCE_DIR}/test)
