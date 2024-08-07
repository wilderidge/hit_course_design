#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <x86intrin.h>
#include <immintrin.h>

#include "public/Utils.h"
#include "public/CheckMatrix.h"

#define M 1000
#define N 1000

//数组转置
void transposeMatrix(double **A, double **At, int m, int n) {
    // 分配内存给转置矩阵 At
    At[0] = malloc(m * n * sizeof(double));
    if (At[0] == NULL) {
        fprintf(stderr, "Memory allocation failed for transposed matrix!\n");
        return;
    }
    for (int i = 1; i < n; i++) {
        At[i] = At[0] + (i * m);
    }

    // 执行转置操作
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            At[j][i] = A[i][j];
        }
    }
}

int CompareVector(double *vec1, double *vec2, int size, int scale) {
    const int blockSize = 4;
    int i = 0;
    __m256d mismatch = _mm256_setzero_pd();
    __m256d diffThreshold = _mm256_set1_pd(DPAR_PRESOLVE_TOL_AIJ);

    __m256d scalarVec = _mm256_set1_pd(scale);

    for (; i <= size - blockSize; i += blockSize) {
        __m256d vecA = _mm256_loadu_pd(&vec1[i]);
        __m256d vecB = _mm256_loadu_pd(&vec2[i]);
        __m256d diff = _mm256_sub_pd(vecA, _mm256_mul_pd(vecB, scalarVec));
        __m256d cmpResult = _mm256_cmp_pd(diff, diffThreshold, _CMP_LT_OQ);
        int mask = _mm256_movemask_pd(cmpResult);
        if (mask != 0b1111) {
            return 0;
        }
    }

    for (; i < size; i++) {
        if (fabs(vec1[i] - vec2[i] * scale) > DPAR_PRESOLVE_TOL_AIJ) {
            return 0;
        }
    }
}

// 比较两个向量比值是否相等
int CompareColumns(double *col1, double *col2, int m) {
    int i = 0;
    double ratio = 0;
    for (; i < m; i++) {
        if ((col2[i] == 0 && col1[i] != 0) || (col1[i] == 0 && col2[i] != 0)) {
            return 0; // 不相等
        }
        if (col2[i] == 0 && col1[i] == 0) {
            continue;
        }
        ratio = col1[i] / col2[i];
        break;
    }
    return CompareVector(col1, col2, m, ratio);
    for (int j = i; j < m; j++) {
        if (col1[j] / col2[j] != ratio && fabs(col1[j] / col2[j] - ratio) > DPAR_PRESOLVE_TOL_AIJ) {
            return 0; // 不相等
        }
    }
    return 1; //相等
}

// 检查矩阵中的重复列
int CheckDuplicatedColumns(double **A, int m, int n, DupColInfo **Cols, int nThread) {
    if (nThread <= 1) {
        // 单线程版本
        int colsCount = 0;
        //flag用来判断一列中是否有
        int flag = 0;
        DupColInfo *tempCols = malloc(n * (n - 1) / 2 * sizeof(DupColInfo));
        if (!tempCols) {
            return -1; // 内存分配失败
        }

        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (CompareColumns(A[i], A[j], m)) {
                    // 找到重复列
                    tempCols[colsCount].jcol = i;
                    tempCols[colsCount].kcol = j;
                    colsCount++;
                }
            }
        }

        // 重新分配内存以适应实际找到的重复列对数量
        *Cols = realloc(tempCols, colsCount * sizeof(DupColInfo));
        if (!*Cols) {
            free(tempCols);
            return -1; // 内存重新分配失败
        }

        return colsCount; // 返回找到的重复列对的数量
    } else {
        // 多线程版本（需要进一步实现）
        // ...
    }
}


int main(int argc, char *argv[]) {
    double **A = malloc(M * sizeof(double *));
    double *pA = malloc(M * N * sizeof(double));
    double **At = malloc(N * sizeof(double *));
    DupColInfo *Cols = malloc(N * sizeof(DupColInfo));;

    if (A == NULL || pA == NULL || Cols == NULL) {
        printf("Malloc matrix failed!\n");
        free(A);
        free(pA);
        free(pA);
        free(Cols);
        return 1;
    }

    // 初始化矩阵A的指针
    for (int i = 0; i < M; i++) {
        A[i] = &(pA[i * N]);

    }

    RandomMatrix(A, M, N);

    // 设置重复列
    for (int i = 0; i < M; i++) {
        A[i][5] = 2 * A[i][4];
        A[i][6] = A[i][3];
    }

    // 转置矩阵 A
    transposeMatrix(A, At, M, N);

    // // 打印矩阵A
    // printf("Matrix A:\n");
    // for (int i = 0; i < M; i++) {
    //     for (int j = 0; j < N; j++) {
    //         printf("%f ", A[i][j]);
    //     }
    //     printf("\n");
    // }

    // // 打印转置后的矩阵 At
    // printf("Transposed Matrix At:\n");
    // for (int i = 0; i < N; i++) {
    //     for (int j = 0; j < M; j++) {
    //         printf("%f ", At[i][j]);
    //     }
    //     printf("\n");
    // }


    // 检测重复列，这里使用单线程
    clock_t start, end;
    start = clock();
    double cpu_time_used;
    int iterations = 500;
    int duplicatedColsCount;
    for (int i = 0; i < iterations; i++)
        duplicatedColsCount = CheckDuplicatedColumns(At, M, N, &Cols, 1);
    end = clock();
    printf("duplicatedColsCount is:%d\n", duplicatedColsCount);

    if (duplicatedColsCount < 0) {
        printf("Failed to check duplicated columns.\n");
    } else if (duplicatedColsCount == 0) {
        printf("No duplicated columns found.\n");
    } else {
        printf("Found %d duplicated column pairs:\n", duplicatedColsCount);
        for (int i = 0; i < duplicatedColsCount; i++) {
            printf("Columns %d and %d are duplicated.\n", Cols[i].jcol, Cols[i].kcol);
        }
    }
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("time used is %f\n", cpu_time_used);
    // 释放内存
    free(A);
    free(At);
    free(pA);
    free(Cols);

    return 0;
}