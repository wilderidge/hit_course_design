#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
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
// 辅助函数：检查元素是否在数组中
int is_in_array(int *arr, int size, int value) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == value) {
            return 1; // 元素在数组中
        }
    }
    return 0; // 元素不在数组中
}

int RemoveSingletonOrEmptyCols(double *pAd,double **A, double **Ad, int m, int n, ColInfo **Cols, int *ColsSize) {
    printf("*ColsSize:%d\n",*ColsSize);
    if (*ColsSize > 0) {
        int s = n - *ColsSize;
        // 分配新矩阵Ad的空间
        pAd = malloc(m * s * sizeof(double));
        if (pAd == NULL) {
            fprintf(stderr, "Memory allocation for new matrix data failed!\n");
            return 0 ;
        }
        
        for (int i = 0; i < m; i++) {
            Ad[i] = &(pAd[i * s]);
        }
        
        int D[n];
        int index = 0;
        for (int j = 0; j < n; j++) {
            if ( (*Cols)[j].type == 2 || (*Cols)[j].type == 3) {
                D[index++] = (*Cols)[j].jcol;
            }
        }

        int new_col_index = 0; // 新矩阵的列索引
        for (int j = 0; j < n; j++) {
            if (!is_in_array(D, index, j)) { // 如果列号 j 不在 D 数组中
                for (int i = 0; i < m; i++) {
                    Ad[i][new_col_index] = A[i][j]; // 复制元素
                    // printf("%f ",A[i][j]);
                    // printf("%f ",Ad[new_col_index][i]);
                }
                new_col_index++; // 增加新矩阵的列索引
            }
        }
        
        return new_col_index; // 成功
    }
}

// 比较两个向量比值是否相等
int CompareRows( double *col1, const double *col2, int m) {
    int i = 0;
    double ratio = 0;
    for (i; i < m; i++) {
        if ((col2[i] == 0 && col1[i] != 0)||(col1[i] == 0 && col2[i] != 0)) {
            return 0; // 不相等
        }
        if(col2[i] == 0 && col1[i] == 0){
            continue;
        }
        ratio = col1[i] / col2[i];
        break;
    }
    for (int j = i; j < m; j++){
        if (col1[j] / col2[j] != ratio  && fabs(col1[j] / col2[j] - ratio) > DPAR_PRESOLVE_TOL_AIJ){
            return 0; // 不相等
        }
    }
    return 1; //相等
}



// 检查矩阵中的重复行
int CheckDuplicatedRows(double **A, int m, int n, DupRowInfo **DupRows, ColInfo **SingletonCols, int *nSingletons, int nThread) {
    if (nThread <= 1) {
        // 单线程版本
        int rowsCount = 0;
        //flag用来判断一列中是否有
        int flag=0;
        DupRowInfo *tempRows = malloc(n * (n - 1) / 2 * sizeof(DupRowInfo));
        if (!tempRows) {
            fprintf(stderr, "error1!\n");
            return -1; // 内存分配失败
        }

        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (CompareRows(A[i], A[j], m)) {
                    // 找到重复列
                    tempRows[rowsCount].irow = i;
                    tempRows[rowsCount].krow = j;
                    rowsCount++;
                }
            }
        }

        // 重新分配内存以适应实际找到的重复列对数量
        *DupRows = realloc(tempRows, rowsCount * sizeof(DupRowInfo));
        // if (!*Cols) {
        //     free(tempRows);
        //     fprintf(stderr, "error2!\n");
        //     return -1; // 内存重新分配失败
        // }

        return rowsCount; // 返回找到的重复行对的数量
    } else {
        // 多线程版本
        // ...
    }
}


int main(int argc, char *argv[]) {
    double **A = malloc(M * sizeof(double *));
    double *B = malloc(M * sizeof(double));
    double *C = malloc(N * sizeof(double));
    double *pA = malloc(M * N * sizeof(double));
    double *pAd = NULL;
    double **Ad = malloc(M * sizeof(double *));
    DupRowInfo *Rows = malloc(M * sizeof(DupRowInfo));
    ColInfo *Cols = malloc(N * sizeof(ColInfo));
    int ColsSize = 0; // 用于存储空列和单元素列的数量

    if (A == NULL || pA == NULL || B == NULL || C == NULL || Rows == NULL || Cols == NULL || Ad == NULL) {
        printf("Malloc matrix failed!\n");
        free(A); free(pA); free(B); free(C); free(pA); free(Rows); free(Cols); free(Ad);
        return 1;
    }
    
    // 初始化矩阵A的指针
    for (int i = 0; i < M; i++) {
        A[i] = &(pA[i * N]);

    }

    RandomMatrix(A, M, N);


    // 设置重复行
    for (int i = 0; i < M; i++) {
        A[5][i] = 2*A[4][i];   
    }

    //设置单元素列
    A[0][2] = 3;
    for (int i = 1; i < M; i++) {
        A[i][2] = 0;
    }


    matrix_check(A, B, C);

    printf("\nC: ");
    for (int j = 0; j < N; j++) {
        printf("%f ", C[j]);
    }
    printf("\n");


    // 打印矩阵A
    printf("Matrix A:\n");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%f ", A[i][j]);
        }
        printf("\n");
    }

    CheckEmptyAndSingletonCols(C, M, N, &Cols, &ColsSize, 1);
    //去除单元素列
    int P = RemoveSingletonOrEmptyCols(pAd,A, Ad, M, N, &Cols, &ColsSize);



    // 打印矩阵Ad
    printf("Matrix Ad:\n");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < P; j++) {
            printf("%f ", Ad[i][j]);
        }
        printf("\n");
    }

    // 检测重复行，这里使用单线程
    int duplicatedRowsCount = CheckDuplicatedRows(A, M, N, &Rows,&Cols, &ColsSize, 1);

    printf("duplicatedColsCount is:%d\n",duplicatedRowsCount);

    if (duplicatedRowsCount < 0) {
        printf("Failed to check duplicated rows.\n");
    } else if (duplicatedRowsCount == 0) {
        printf("No duplicated rows found.\n");
    } else {
        printf("Found %d duplicated row pairs:\n", duplicatedRowsCount);
        for (int i = 0; i < duplicatedRowsCount; i++) {
            printf("Rows %d and %d are duplicated.\n", Rows[i].irow, Rows[i].krow);
        }
    }

    // 释放内存
    free(A);
    free(pA);
    free(B);
    free(C);
    free(Ad);
    free(Rows);
    free(Cols);

    return 0;
}
