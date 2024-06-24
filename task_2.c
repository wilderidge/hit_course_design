#include <stdio.h>
#include <stdlib.h>
#include "Utils.h"
#include "CheckMatrix.h"

#define M 10
#define N 10


int CheckDuplicatedColumns(double **A, int m, int n, DupColInfo *Cols, int nThread)
{
    if(nThread == 1)
    {
        
    }
    else
    {
        // 多线程处理逻辑
    }
}

#if 1
int main(int argc, char *argv[])
{
    double **A = malloc(M * sizeof(double *));
    double *pA = malloc(M * N * sizeof(double));

    if (A == NULL || pA == NULL) {
        printf("Malloc matrix failed!\n");
        free(A);free(pA);
        return 1;
    }

    for (int i = 0; i < M; i++) {
        A[i] = &(pA[i * N]);
    }

    RandomMatrix(A, M, N);

    free(A);
    free(pA);

    return 0;
}
#endif