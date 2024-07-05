#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "Utils.h"
#include <string.h>

#define ZERO_PROB 0.5 // 20%的概率生成0

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

int CheckEmptyAndSingletonRows(double *B, int m, int n, RowInfo **Rows, int *RowsSize ,int nThread)
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


/******************************************************************************
 读取特定矩阵文件

    参数：
        A: M*N 矩阵
        M, N：矩阵维度
        filename:矩阵文件的路径
******************************************************************************/
void ReadMatrix(double **A, int M, int N, char* filename) {
    FILE *fp = fopen(filename, "rb"); // 以二进制模式打开文件
    if (fp == NULL) {
        fprintf(stderr, "无法打开文件 %s\n", filename);
        exit(1);
    }

    for (int i = 0; i < M; i++) {
        // 为矩阵的每一行分配内存
        A[i] = (double *)malloc(N * sizeof(double));
        if (A[i] == NULL) {
            fprintf(stderr, "内存分配失败\n");
            fclose(fp); // 确保在退出前关闭文件
            exit(1);
        }

        // 从文件中读取一行数据
        size_t readCount = fread(A[i], sizeof(double), N, fp);
        // for(int k = 0;k<N;k++)
        //     printf("%lf\t\t", A[i][k]);
        // printf("\n");
        if (readCount < N) {
            fprintf(stderr, "文件格式错误或提前结束\n");
            fclose(fp); // 确保在退出前关闭文件
            exit(1);
        }
    }

    fclose(fp); // 完成读取后关闭文件
}


/******************************************************************************
 生成随机矩阵

    参数：
        A: M*N 矩阵
        M, N：矩阵维度
******************************************************************************/
void RandomMatrix(double **A, int M, int N)
{
    int     i, j;
    double  Range = (double)(UPPER_BOUND - LOWER_BOUND);

    srand(time(NULL));

    for(i = 0; i < M; i++) {
        for(j = 0; j < N; j++) {
            if ((double)rand() / RAND_MAX < ZERO_PROB) {
                A[i][j] = 0; // 有ZERO_PROB的概率设置为0
            } else {
                A[i][j] = (double)rand() / RAND_MAX * Range + LOWER_BOUND;
            }
        }
    }
}

/******************************************************************************
 生成随机向量

    参数：
        a: N维向量 
        N：向量维度
******************************************************************************/
void RandomVector(double *a, int N)
{
    int     i;
    double  Range = (double)(UPPER_BOUND - LOWER_BOUND);

    srand(time(NULL));

    for(i = 0; i < N; i++)
        a[i] = (double)rand() / RAND_MAX * Range + LOWER_BOUND;
}

/******************************************************************************
 矩阵-矩阵乘法线程函数

    参数：
        arg: 传入参数(MatMulThreadInfo)指针
******************************************************************************/
void *Thread_Matrix_Matrix_Multiplication_Func(void *arg)                                  
{
    MatMulThreadInfo        MatInfo = *(MatMulThreadInfo *)arg;
    int                     i, j, k;
    double                  Sum;

    for(i = 0; i < MatInfo.M; i++)
        for(j = 0; j < MatInfo.N; j++)
        {
            Sum = 0.0;

            for(k = 0; k < MatInfo.K; k++)
                Sum += MatInfo.A[i][k] * MatInfo.B[k][j];

            MatInfo.C[i][j] = Sum;
        }

    return NULL;
}

/******************************************************************************
 矩阵-向量乘法线程函数

    参数：
        arg: 传入参数(MatMulThreadInfo)指针
******************************************************************************/
void *Thread_Matrix_Vector_Multiplication_Func(void *arg)                                  
{
    MatMulThreadInfo        MatInfo = *(MatMulThreadInfo *)arg;
    int                     i, j;
    double                  Sum;

    for(i = 0; i < MatInfo.M; i++)
    {
        MatInfo.b[i] = 0;
        for(j = 0; j < MatInfo.N; j++)
            MatInfo.b[i] += MatInfo.A[i][j] * MatInfo.x[j];
    }

    return NULL;
}

/******************************************************************************
 矩阵-矩阵乘法函数
    计算： 
            C = A * B

    参数：
        A: M*K 矩阵
        B: K*N 矩阵
        C: M*N 矩阵
        M, K, N：矩阵维度
        nThread：线程数，<1--无法计算，=1--单线程串行计算，>1--多线程计算

    返回：
        0：正常返回
        1：线程数设置过小，无法计算
        2：申请内存失败
        3：建立线程失败
******************************************************************************/
int Matrix_Matrix_Multiplication(double **A, double **B, double **C, int M, int K, int N, int nThread)
{
    int                 i, j, k, L;
    double              Sum;
    MatMulThreadInfo    *pThreadInfo;               //线程参数结构
    pthread_t           *ptThread;                  //线程handle

    if(nThread < 1)
    {
        //无线程，无法计算
        return 1;
    }
    else if(nThread == 1)
    {
        //单线程，串行计算

        for(i = 0; i < M; i++)
            for(j = 0; j < N; j++)
            {
                Sum = 0.0;

                for(k = 0; k < K; k++)
                    Sum += A[i][k] * B[k][j];

                C[i][j] = Sum;
            }
    }
    else
    {
        //多线程计算

        pThreadInfo = (MatMulThreadInfo *)malloc(nThread * sizeof(MatMulThreadInfo));
        ptThread = (pthread_t *)malloc(nThread * sizeof(pthread_t *));

        if((pThreadInfo == NULL) || (ptThread == NULL))
            return 2;

        L = M / nThread;            //按设置的线程数分配工作块（单个线程所要计算的行数L）

        for(i = 0; i < nThread; i++)
        {
            //分配线程任务
            pThreadInfo[i].A = &A[i*L];
            pThreadInfo[i].B = B;
            pThreadInfo[i].C = &C[i*L];

            if(i != nThread-1)
                pThreadInfo[i].M = L;
            else
            {
                pThreadInfo[i].M = M - (nThread-1)*L;
            }

            pThreadInfo[i].K = K;
            pThreadInfo[i].N = N;

            //启动线程
            if(pthread_create(&ptThread[i], NULL, Thread_Matrix_Matrix_Multiplication_Func, (void *)&(pThreadInfo[i])))
                return 3;
        }

        for(i = 0; i < nThread; i++)
            pthread_join(ptThread[i], NULL);                //等所有的子线程计算结束

        free(pThreadInfo);
        free(ptThread);
    }

    return 0;
}

/******************************************************************************
 矩阵-向量乘法函数
    计算： 
            b = A * x

    参数：
        A: M*N 矩阵
        x: N 维向量
        b: M 维向量
        M, N：矩阵、向量维度
        nThread：线程数，<1--无法计算，=1--单线程串行计算，>1--多线程计算

    返回：
        0：正常返回
        1：线程数设置过小，无法计算
        2：申请内存失败
        3：建立线程失败
******************************************************************************/
int Matrix_Vector_Multiplication(double **A, double *x, double *b, int M, int N, int nThread)
{
    int                 i, j, L;
    MatMulThreadInfo    *pThreadInfo;               //线程参数结构
    pthread_t           *ptThread;                  //线程handle

    if(nThread < 1)
    {
        //无线程，无法计算
        return 1;
    }
    else if(nThread == 1)
    {
        //单线程，串行计算

        for(i = 0; i < M; i++)
        {
            b[i] = 0.0;
            for(j = 0; j < N; j++)
                b[i] += A[i][j] * x[j];
        }
    }
    else
    {
        //多线程计算

        pThreadInfo = (MatMulThreadInfo *)malloc(nThread * sizeof(MatMulThreadInfo));
        ptThread = (pthread_t *)malloc(nThread * sizeof(pthread_t *));

        if((pThreadInfo == NULL) || (ptThread == NULL))
            return 2;

        L = M / nThread;            //按设置的线程数分配工作块（单个线程所要计算的行数L）

        for(i = 0; i < nThread; i++)
        {
            //分配线程任务
            pThreadInfo[i].A = &A[i*L];
            pThreadInfo[i].x = x;
            pThreadInfo[i].b = &b[i*L];

            if(i != nThread-1)
                pThreadInfo[i].M = L;
            else
            {
                pThreadInfo[i].M = M - (nThread-1)*L;
            }

            pThreadInfo[i].N = N;

            //启动线程
            if(pthread_create(&ptThread[i], NULL, Thread_Matrix_Vector_Multiplication_Func, (void *)&(pThreadInfo[i])))
                return 3;
        }

        for(i = 0; i < nThread; i++)
            pthread_join(ptThread[i], NULL);                //等所有的子线程计算结束

        free(pThreadInfo);
        free(ptThread);
    }

    return 0;
}
