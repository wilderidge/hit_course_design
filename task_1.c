#include <stdio.h>
#include <stdlib.h>
#include "public/Utils.h"
#include "public/CheckMatrix.h"

#define M 10
#define N 10

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
        // 多线程处理逻辑
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

    CheckEmptyAndSingletonRows(B, M, N, &Rows, &RowsSize, 1);
    printf("RowsSize: %d\n", RowsSize);
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