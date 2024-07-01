#pragma once
#define LOWER_BOUND     -10
#define UPPER_BOUND     +10

#include "global_settings.h"

typedef struct {
    double      **A;
    double      **B;
    double      **C;
    double      *x;
    double      *b;
    int         M;
    int         K;
    int         N;
}MatMulThreadInfo;

void RandomMatrix(double **A, int M, int N);        //随机生成矩阵
void RandomVector(double *a, int N);                //随机生成向量
int Matrix_Matrix_Multiplication(double **A, double **B, double **C, int M, int K, int N, int nThread);     //单线程、多线程矩阵-矩阵乘法
int Matrix_Vector_Multiplication(double **A, double *x, double *b, int M, int N, int nThread);              //单线程、多线程矩阵-向量乘法
