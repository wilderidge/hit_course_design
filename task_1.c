#include <stdio.h>
#include <stdlib.h>
#include "Utils.h"

#define M 3000
#define N 3000

void matrix_check(double **A, double *B, double *C)
{
    // 初始化B和C向量
    for (int i = 0; i < M; i++) {
        B[i] = 0;
    }
    for (int j = 0; j < N; j++) {
        C[j] = 0;
    }

    // 遍历矩阵A的每一行和每一列
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (A[i][j] != 0) {
                B[i] += 1; // 如果当前行的元素不为0，增加B[i]
                C[j] += 1; // 如果当前列的元素不为0，增加C[j]
            }
        }
    }
}

int main(int argc, char *argv[])
{
    double **A = malloc(M * sizeof(double *));
    double *B = malloc(M * sizeof(double));
    double *C = malloc(N * sizeof(double));
    double *pA = malloc(M * N * sizeof(double));

    if (A == NULL || B == NULL || C == NULL || pA == NULL) {
        printf("Malloc matrix failed!\n");
        return 1;
    }

    for (int i = 0; i < M; i++) {
        A[i] = &(pA[i * N]);
    }

    RandomMatrix(A, M, N);

    matrix_check(A, B, C);

    printf("B: ");
    for (int i = 0; i < M; i++) {
        printf("%f ", B[i]);
    }
    printf("\nC: ");
    for (int j = 0; j < N; j++) {
        printf("%f ", C[j]);
    }
    printf("\n");

    free(A);
    free(B);
    free(C);
    free(pA);

    return 0;
}