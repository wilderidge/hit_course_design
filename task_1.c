#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "public/Utils.h"
#include "public/CheckMatrix.h"

int main(int argc, char *argv[]) {
    char *filename = "../A(2262x9799).80bau3b.bin";
    int M = 2262;
    int N = 9799;
    if (argc == 4) {
        filename = argv[1];
        M = atoi(argv[2]);
        N = atoi(argv[3]);
    } else if (argc != 4 && argc != 1)
        return 0;

    printf("## N: %d, M: %d\n", N, M);

    int nThread = 8;             //线程数
    double total_time_used = 0.0;
    int iterations = 100;        //执行测试的次数

    double **A = malloc(M * sizeof(double *));
    double *B = malloc(M * sizeof(double));
    double *C = malloc(N * sizeof(double));
    double *pA = malloc(M * N * sizeof(double));
    RowInfo *Rows = malloc(M * sizeof(RowInfo));
    ColInfo *Cols = malloc(N * sizeof(ColInfo));
    int RowsSize = 0; // 用于存储空行和单元素行的数量
    int ColsSize = 0; // 用于存储空列和单元素列的数量

    if (A == NULL || B == NULL || C == NULL || pA == NULL || Rows == NULL || Cols == NULL) {
        printf("Malloc matrix failed!\n");
        free(A);
        free(B);
        free(C);
        free(pA);
        free(Rows);
        free(Cols);
        return 1;
    }

    for (int i = 0; i < M; i++) {
        A[i] = &(pA[i * N]);
    }

    // RandomMatrix(A, M, N);
    ReadMatrix(A, M, N, filename);
    for (int iter = 0; iter < iterations; iter++) {
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        RowsSize = 0;
        ColsSize = 0;

        matrix_check(A, B, C, 1, M, N, 1);

        CheckEmptyAndSingletonRows(B, M, N, &Rows, &RowsSize, 1);
        // printf("RowsSize: %d\n", RowsSize);

        CheckEmptyAndSingletonCols(C, M, N, &Cols, &ColsSize, 1);
        // printf("ColsSize: %d\n", ColsSize);

        end = clock();

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_time_used += cpu_time_used;
    }

    double average_time_used = total_time_used / iterations;
    printf("1. M: %d, N: %d, 单线程平均函数执行耗时: %f 秒\n", M, N, average_time_used);

    total_time_used = 0;
    for (int iter = 0; iter < iterations; iter++) {
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        RowsSize = 0;
        ColsSize = 0;

        matrix_check(A, B, C, 1, M, N, 0);

        CheckEmptyAndSingletonRows(B, M, N, &Rows, &RowsSize, 1);
        // printf("RowsSize: %d\n", RowsSize);

        CheckEmptyAndSingletonCols(C, M, N, &Cols, &ColsSize, 1);
        // printf("ColsSize: %d\n", ColsSize);

        end = clock();

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_time_used += cpu_time_used;
    }

    average_time_used = total_time_used / iterations;
    printf("2. M: %d, N: %d, 单线程二循环展开平均函数执行耗时: %f 秒\n", M, N, average_time_used);


    total_time_used = 0;
    for (int iter = 0; iter < iterations; iter++) {
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        RowsSize = 0;
        ColsSize = 0;

        matrix_check(A, B, C, nThread, M, N, 1);

        CheckEmptyAndSingletonRows(B, M, N, &Rows, &RowsSize, 1);
        // printf("RowsSize: %d\n", RowsSize);

        CheckEmptyAndSingletonCols(C, M, N, &Cols, &ColsSize, 1);
        // printf("ColsSize: %d\n", ColsSize);

        end = clock();

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_time_used += cpu_time_used;
    }

    average_time_used = total_time_used / iterations;
    printf("3. M: %d, N: %d, 八线程平均函数执行耗时: %f 秒\n\n", M, N, average_time_used);

    free(A);
    free(B);
    free(C);
    free(pA);
    free(Rows);
    free(Cols);

    return 0;
}