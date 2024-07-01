API : public/CheckMatrix.h

```C
int CheckEmptyAndSingletonRows(double **A, int m, int n, RowInfo *Rows, int nThread);
int CheckEmptyAndSingletonCols(double **A, int m, int n, ColInfo *Cols, int nThread);
int CheckDuplicatedColumns(double **A, int m, int n, DupColInfo *Cols, int nThread);
int CheckDuplicatedRows(double **A, int m, int n, DupRowInfo *DupRows, ColInfo *SingletonCols, int nSingletons, int nThread);
```

|函数|完成|测试|
|:---|:--:|:--:|
|CheckEmptyAndSingletonRows|OK, pthread||
|CheckEmptyAndSingletonCols|OK, pthread||
|CheckDuplicatedColumns|OK||
|CheckDuplicatedRows|OK||
