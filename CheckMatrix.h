
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


// int CheckEmptyAndSingletonRows(double **A, int m, int n, RowInfo *Rows, int nThread);
// int CheckEmptyAndSingletonCols(double **A, int m, int n, ColInfo *Cols, int nThread);
// int CheckDuplicatedColumns(double **A, int m, int n, DupColInfo *Cols, int nThread);
// int CheckDuplicatedRows(double **A, int m, int n, DupRowInfo *DupRows, ColInfo *SingletonCols, int nSingletons, int nThread);
