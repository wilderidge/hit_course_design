#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include "public/Utils.h"
#include "public/CheckMatrix.h"

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

// 比较两个向量比值是否相等
int CompareColumns(double *col1, double *col2, int m) {
    int i = 0;
    double ratio = 0;
    for (i; i < m; i++) {
        if ((col2[i] == 0 && col1[i] != 0) || (col1[i] == 0 && col2[i] != 0)) {
            return 0; // 不相等
        }
        if (col2[i] == 0 && col1[i] == 0) {
            continue;
        }
        ratio = col1[i] / col2[i];
        break;
    }

    for (int j = i + 1; j < m; j++) {
        if (fabs(col1[j] - col2[j] * ratio) > DPAR_PRESOLVE_TOL_AIJ) {
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
                    //printf("Columns %d and %d are duplicated.\n", i,j);
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

        // 多线程版本
        //printf("nthread!\n");

        int colsCount = 0;
        DupColInfo *tempCols = malloc(n * (n - 1) / 2 * sizeof(DupColInfo));
        if (!tempCols) {
            return -1; // 内存分配失败
        }


#pragma omp parallel num_threads(nThread)
        {
            int tid = omp_get_thread_num();
#pragma omp for schedule(dynamic, 1) reduction(+:colsCount)
            for (int i = 0; i < n; i++) {
                // 动态分配任务,每次分配1行
                for (int j = i + 1; j < n; j++) {
                    if (CompareColumns(A[i], A[j], m)) {
#pragma omp critical
                        {
                            tempCols[colsCount].jcol = i;
                            tempCols[colsCount].kcol = j;
                            //printf("Columns %d and %d are duplicated.\n", i,j);
                            colsCount++;
                        }
                    }
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

    }
}


int main(int argc, char *argv[]) {
    char *filename = "./A(2262x9799).80bau3b.bin";
    int M = 10000;
    int N = 10000;

    double total_time_used = 0.0;
    int iterations = 1;        //执行测试的次数


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
    //ReadMatrix(A, M, N, filename);

    //设置重复列
    for (int i = 0; i < M; i++) {
        A[i][5] = 2 * A[i][4];
        A[i][6] = A[i][3];
        A[i][263] = A[i][33];
        A[i][400] = A[i][8];
        A[i][666] = A[i][55];

    }

    // 转置矩阵 A
    transposeMatrix(A, At, M, N);

    //单线程测试循环
    for (int iter = 0; iter < iterations; iter++) {
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        // 检测重复列
        int duplicatedColsCount = CheckDuplicatedColumns(At, M, N, &Cols, 1);

        end = clock();

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_time_used += cpu_time_used;
    }

    double average_time_used = total_time_used / iterations;
    printf("M: %d ,N: %d ,单线程平均函数执行耗时: %f 秒\n", M, N, average_time_used);


    total_time_used = 0;

    //2线程测试循环
    for (int iter = 0; iter < iterations; iter++) {
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        // 检测重复列
        int duplicatedColsCount = CheckDuplicatedColumns(At, M, N, &Cols, 2);

        end = clock();

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_time_used += cpu_time_used;
    }

    average_time_used = total_time_used / iterations;
    printf("M: %d ,N: %d ,二线程平均函数执行耗时: %f 秒\n", M, N, average_time_used);

    total_time_used = 0;


    //4线程测试循环
    for (int iter = 0; iter < iterations; iter++) {
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        // 检测重复列
        int duplicatedColsCount = CheckDuplicatedColumns(At, M, N, &Cols, 4);

        end = clock();

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_time_used += cpu_time_used;
    }

    average_time_used = total_time_used / iterations;
    printf("M: %d ,N: %d ,四线程平均函数执行耗时: %f 秒\n", M, N, average_time_used);


    total_time_used = 0;


    //8线程测试循环
    for (int iter = 0; iter < iterations; iter++) {
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        // 检测重复列
        int duplicatedColsCount = CheckDuplicatedColumns(At, M, N, &Cols, 8);

        end = clock();

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_time_used += cpu_time_used;
    }

    average_time_used = total_time_used / iterations;
    printf("M: %d ,N: %d ,八线程平均函数执行耗时: %f 秒\n", M, N, average_time_used);

    //if (duplicatedColsCount < 0) {
    //    printf("Failed to check duplicated columns.\n");
    //} else if (duplicatedColsCount == 0) {
    //    printf("No duplicated columns found.\n");
    //} else {
    //    printf("Found %d duplicated column pairs:\n", duplicatedColsCount);
    //}

    // 释放内存
    free(A);
    free(At);
    free(pA);
    free(Cols);

    return 0;
}
