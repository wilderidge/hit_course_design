#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "public/Utils.h"
#include "public/CheckMatrix.h"

#define M 10000
#define N 10000

void *thread_function_check(void *arg) {
    ThreadData_check_task1 *data = (ThreadData_check_task1 *)arg;
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < N; j++) {
            if (data->A[i][j] != 0) {
                data->B[i] += 1;
                data->C[j] += 1;
            }
        }
    }
    pthread_exit(NULL);
}

void *threadFunction(void *arg) {
    ThreadData_task1 *data = (ThreadData_task1 *)arg;
    data->localRowsSize = 0;
    for (int i = data->startIdx; i < data->endIdx; i++) {
        if (data->B[i] == 0 || data->B[i] == 1) {
            data->localRows[data->localRowsSize].type = (data->B[i] == 0) ? EMPTY_ROW : SINGLETON_ROW;
            data->localRows[data->localRowsSize].irow = i;
            data->localRowsSize++;
        }
    }
    pthread_exit(NULL);
}

void matrix_check(double **A, double *B, double *C, int nThread)
{   //经试验，多线程带来的性能提升无法抵消多线程带来的额外开销，故此处最好是用单线程
    if (nThread == 1)
    {
        for (int i = 0; i < M; i++) {
            B[i] = 0;
        }
        for (int j = 0; j < N; j++) {
            C[j] = 0;
        }
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (A[i][j] != 0) {
                    B[i] += 1;
                    C[j] += 1;
                }
            }
        }
    }
    else
    {
        pthread_t threads[nThread];
        ThreadData_check_task1 thread_data[nThread];
        int rows_per_thread = M / nThread;
        for (int i = 0; i < nThread; i++) {
            thread_data[i].start_row = i * rows_per_thread;
            thread_data[i].end_row = (i + 1) * rows_per_thread;
            thread_data[i].A = A;
            thread_data[i].B = B;
            thread_data[i].C = C;
            pthread_create(&threads[i], NULL, thread_function_check, (void *)&thread_data[i]);
        }
        for (int i = 0; i < nThread; i++) {
            pthread_join(threads[i], NULL);
        }
    }
}

int CheckEmptyAndSingletonRows(double *B, int m, int n, RowInfo **Rows, int *RowsSize ,int nThread )
{
    if (nThread == 1)
    {
        *RowsSize = 0;
        *Rows = malloc(n * sizeof(RowInfo)); // 预分配足够的空间
        if (*Rows == NULL) {
            return -1;
        }

        for (int i = 0; i < n; i++)
        {
            if (B[i] == 0 || B[i] == 1)
            {
                (*Rows)[*RowsSize].type = (B[i] == 0) ? EMPTY_ROW : SINGLETON_ROW;
                (*Rows)[*RowsSize].irow = i;
                (*RowsSize)++;
            }
        }

        // 根据实际找到的列数重新调整 Rows 数组的大小
        *Rows = realloc(*Rows, (*RowsSize) * sizeof(RowInfo));
        if (*Rows == NULL && *RowsSize > 0) {
            return -1;
        }
    }
    else
    {
        pthread_t threads[nThread];
        ThreadData_task1 threadData[nThread];
        int segmentSize = n / nThread;
        for (int i = 0; i < nThread; i++) {
            threadData[i].B = B;
            threadData[i].startIdx = i * segmentSize;
            threadData[i].endIdx = (i == nThread - 1) ? n : (i + 1) * segmentSize;
            threadData[i].localRows = malloc(n * sizeof(RowInfo)); // 预分配足够的空间
            pthread_create(&threads[i], NULL, threadFunction, (void *)&threadData[i]);
        }
        *RowsSize = 0;
        *Rows = malloc(n * sizeof(RowInfo)); // 预分配足够的空间
        for (int i = 0; i < nThread; i++) {
            pthread_join(threads[i], NULL);
            for (int j = 0; j < threadData[i].localRowsSize; j++) {
                (*Rows)[*RowsSize] = threadData[i].localRows[j];
                (*RowsSize)++;
            }
            free(threadData[i].localRows);
        }
    }
    return 0;
}

int CheckEmptyAndSingletonCols(double *C, int m, int n, ColInfo **Cols, int *ColsSize, int nThread)
{
    if (nThread == 1)
    {
        *ColsSize = 0;
        *Cols = malloc(n * sizeof(ColInfo));
        if (*Cols == NULL) {
            return -1;
        }

        for (int j = 0; j < n; j++)
        {
            if (C[j] == 0 || C[j] == 1)
            {
                (*Cols)[*ColsSize].type = (C[j] == 0) ? EMPTY_COL : SINGLETON_COL;
                (*Cols)[*ColsSize].jcol = j;
                (*ColsSize)++;
            }
        }

        // 根据实际找到的列数重新调整 Cols 数组的大小
        *Cols = realloc(*Cols, (*ColsSize) * sizeof(ColInfo));
        if (*Cols == NULL && *ColsSize > 0) {
            return -1;
        }
    }
    else
    {
        // 多线程处理逻辑
    }
    return 0;
}

int main(int argc, char *argv[])
{
    clock_t start, end;
    double cpu_time_used;

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
        free(A); free(B); free(C); free(pA); free(Rows); free(Cols);
        return 1;
    }

    for (int i = 0; i < M; i++) {
        A[i] = &(pA[i * N]);
    }

    RandomMatrix(A, M, N);

    matrix_check(A, B, C, 1);

    // start = clock();
    // matrix_check(A, B, C, 2);
    // end = clock();

    // cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    // printf("函数执行耗时: %f 秒\n", cpu_time_used);

    // printf("B: ");
    // for (int i = 0; i < M; i++) {
    //     printf("%f ", B[i]);
    // }
    // printf("\nC: ");
    // for (int j = 0; j < N; j++) {
    //     printf("%f ", C[j]);
    // }
    // printf("\n");

    start = clock();

    CheckEmptyAndSingletonRows(B, M, N, &Rows, &RowsSize, 1);
    printf("RowsSize: %d\n", RowsSize);
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("函数执行耗时: %f 秒\n", cpu_time_used);
    CheckEmptyAndSingletonCols(C, M, N, &Cols, &ColsSize, 1);
    printf("ColsSize: %d\n", ColsSize);

    free(A);
    free(B);
    free(C);
    free(pA);
    free(Rows);
    free(Cols);

    return 0;
}