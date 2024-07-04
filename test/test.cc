#include <filesystem>
#include <gtest/gtest.h>
#include "testTools.h"


int main(int argc, char **argv) {

    MatrixTest::testAll(std::filesystem::current_path() / "data");
    // ::testing::InitGoogleTest(&argc, argv);
    // return RUN_ALL_TESTS();
}