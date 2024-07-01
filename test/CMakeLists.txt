find_package(GTest)

include_directories(
    ${PROJECT_SOURCE_DIR}/test
)

add_executable(test ${PROJECT_SOURCE_DIR}/test/test.cc ${PROJECT_SOURCE_DIR}/test/testTools.cc ${SRC_LIST})

target_link_libraries(test PUBLIC
    gtest
    pthread 
)
