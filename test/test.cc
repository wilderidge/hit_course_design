#include <cstdlib>
#include <gtest/gtest.h>
#include "../public/Utils.h"
#include "../public/Utils.c"

TEST(Utils, func_RandomMatrix) {

    uint32_t zeroCount = 0;
    // Environment
    constexpr int M = 10;
    constexpr int N = 10;

    double **A = (double **)malloc(M * sizeof(double *));
    double *pA = (double *)malloc(M * N * sizeof(double));
    for (int i = 0; i < M; i++) {
        A[i] = &(pA[i * N]);
    }

    // all numbers fall in bound
    RandomMatrix(A, M, N);
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            EXPECT_GE(*(A[i] + j), LOWER_BOUND);
            EXPECT_LE(*(A[i] + j), UPPER_BOUND);
            zeroCount += !!(*(A[i] + j)) ? 0 : 1;
        }
    }
    
    free(A);
    free(pA);

    // zero-probability in [0.0, 0.4]
    EXPECT_GE((double)zeroCount / (M * N), 0.0f);
    EXPECT_LE((double)zeroCount / (M * N), 0.4f);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}