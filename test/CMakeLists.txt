find_package(GTest)

add_executable(test ${PROJECT_SOURCE_DIR}/test/test.cc ${SRC_LIST})

target_link_libraries(test PUBLIC
    gtest
    pthread 
)