#include "CheckMatrix.h"

void matrix_check(double **A, double *B, double *C);
int CheckEmptyAndSingletonRows(double *B, int m, int n, RowInfo **Rows, int *RowsSize ,int nThread);
int CheckEmptyAndSingletonCols(double *C, int m, int n, ColInfo **Cols, int *ColsSize, int nThread);
int CheckDuplicatedColumns(double **A, int m, int n, DupColInfo **Cols, int nThread);
int CheckDuplicatedRows(double **A, int m, int n, DupRowInfo **DupRows, ColInfo **SingletonCols, int *nSingletons, int nThread);