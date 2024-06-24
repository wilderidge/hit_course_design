#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "Utils.h"

/******************************************************************************
 生成随机矩阵

    参数：
        A: M*N 矩阵
        M, N：矩阵维度
******************************************************************************/
#define ZERO_PROB 0.2 // 20%的概率生成0
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