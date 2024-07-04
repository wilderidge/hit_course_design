#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "public/Utils.h"
#include "public/CheckMatrix.h"

void *thread_function_check(void *arg) {
    ThreadData_check_task1 *data = (ThreadData_check_task1 *)arg;
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->N; j++) { // 使用 data->N 而不是全局的 N
            if (data->A[i][j] != 0) {
                data->B[i] += 1;
                data->C[j] += 1;
            }
        }
    }
    pthread_exit(NULL);
}

void *threadFunction_Rows(void *arg) {
    ThreadData_task_Rows *data = (ThreadData_task_Rows *)arg;
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

void* threadFunction_Cols(void *arg) {
    ThreadData_task_Cols *data = (ThreadData_task_Cols *)arg;
    data->partialSize = 0;
    for (int j = data->startCol; j < data->endCol; j++) {
        if (data->C[j] == 0 || data->C[j] == 1) {
            data->partialCols[data->partialSize].type = (data->C[j] == 0) ? EMPTY_COL : SINGLETON_COL;
            data->partialCols[data->partialSize].jcol = j;
            data->partialSize++;
        }
    }
    pthread_exit(NULL);
}

void matrix_check(double **A, double *B, double *C, int nThread, int M, int N, int flag)
{
    if (nThread == 1)
    {
        memset(B, 0, M * sizeof(double));
        memset(C, 0, N * sizeof(double));
        if(flag)
        {
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    if (A[i][j] != 0) {
                        B[i] += 1;
                        C[j] += 1;
                    }
                }
            }
        }
        else{
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j += 2) { // 每次迭代处理2个元素
                    if (A[i][j] != 0) {
                        B[i] += 1;
                        C[j] += 1;
                    }
                    if (j + 1 < N && A[i][j + 1] != 0) { // 检查并处理第二个元素
                        B[i] += 1;
                        C[j + 1] += 1;
                    }
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
            thread_data[i].N = N;
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
        ThreadData_task_Rows threadData[nThread];
        int segmentSize = n / nThread;
        for (int i = 0; i < nThread; i++) {
            threadData[i].B = B;
            threadData[i].startIdx = i * segmentSize;
            threadData[i].endIdx = (i == nThread - 1) ? n : (i + 1) * segmentSize;
            threadData[i].localRows = malloc(n * sizeof(RowInfo)); // 预分配足够的空间
            pthread_create(&threads[i], NULL, threadFunction_Rows, (void *)&threadData[i]);
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
        *ColsSize = 0;
        *Cols = malloc(n * sizeof(ColInfo));
        if (*Cols == NULL) {
            return -1;
        }

        pthread_t threads[nThread];
        ThreadData_task_Cols threadData[nThread];
        int colsPerThread = n / nThread;
        int extraCols = n % nThread;

        for (int i = 0; i < nThread; i++) {
            int startCol = i * colsPerThread;
            int endCol = startCol + colsPerThread + (i < extraCols ? 1 : 0);
            threadData[i] = (ThreadData_task_Cols){C, startCol, endCol, malloc((endCol - startCol) * sizeof(ColInfo)), 0};
            pthread_create(&threads[i], NULL, threadFunction_Cols, &threadData[i]);
        }

        for (int i = 0; i < nThread; i++) {
            pthread_join(threads[i], NULL);
            for (int j = 0; j < threadData[i].partialSize; j++) {
                (*Cols)[*ColsSize] = threadData[i].partialCols[j];
                (*ColsSize)++;
            }
            free(threadData[i].partialCols);
        }

        *Cols = realloc(*Cols, (*ColsSize) * sizeof(ColInfo));
        if (*Cols == NULL && *ColsSize > 0) {
            return -1;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    char *filename = "../A(2262x9799).80bau3b.bin";
    int M = 2262;
    int N = 9799;
    if(argc == 4)
    {
        filename = argv[1];
        M = atoi(argv[2]);
        N = atoi(argv[3]);
    }
    else if(argc != 4 && argc != 1)
        return 0;

    int nThread = 8;             //线程数
    double total_time_used = 0.0;
    int iterations = 500;        //执行测试的次数

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
    printf("M: %d, N: %d, 单线程平均函数执行耗时: %f 秒\n", M,N,average_time_used);

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
    printf("M: %d, N: %d, 单线程二循环展开平均函数执行耗时: %f 秒\n", M,N,average_time_used);


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
    printf("M: %d, N: %d, 八线程平均函数执行耗时: %f 秒\n\n", M,N,average_time_used);

    free(A);
    free(B);
    free(C);
    free(pA);
    free(Rows);
    free(Cols);

    return 0;
}