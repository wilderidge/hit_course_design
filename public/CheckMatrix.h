#pragma once
#define DPAR_PRESOLVE_TOL_AIJ 1.0e-12

#define EMPTY_ROW       0
#define SINGLETON_ROW   1
#define EMPTY_COL       2
#define SINGLETON_COL   3

typedef struct {
    int     type;
    int     irow;
    int     jcol;
}RowInfo;

typedef struct {
    int     type;
    int     jcol;
    int     irow;
}ColInfo;

typedef struct {
    int     irow;
    int     krow;
    double  v;
}DupRowInfo;

typedef struct{
    int     jcol;
    int     kcol;
    double  v;
}DupColInfo;

typedef struct {
    int start_row;
    int end_row;
    double **A;
    double *B;
    double *C;
    int N;
} ThreadData_check_task1;

typedef struct {
    double *B;
    int startIdx;
    int endIdx;
    RowInfo *localRows;
    int localRowsSize;
} ThreadData_task_Rows;

typedef struct {
    double *C;
    int startCol;
    int endCol;
    ColInfo *partialCols;
    int partialSize;
} ThreadData_task_Cols;



int CheckEmptyAndSingletonRows(double *B, int m, int n, RowInfo **Rows, int *RowsSize ,int nThread);
int CheckEmptyAndSingletonCols(double *C, int m, int n, ColInfo **Cols, int *ColsSize, int nThread);
int CheckDuplicatedRows(double **A, int m, int n, DupRowInfo **DupRows, ColInfo **SingletonCols, int *nSingletons, int nThread);
int CheckDuplicatedColumns(double **A, int m, int n, DupColInfo **Cols, int nThread);
