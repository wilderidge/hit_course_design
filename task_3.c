#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "public/Utils.h"
#include "public/CheckMatrix.h"


#define M 700
#define N 1000


// 辅助函数,用于 bsearch 中的比较
int compare_ints(const void *a, const void *b) {
    return *(int *)a - *(int *)b;
}

int CompareRows(double *row1, const double *row2, int D[], int n, int index) {
    double ratio = 0;
    int i, j;
    int first_nonzero = -1;
    void *found;

    // 找到第一个非零元素
    for (i = 0; i < n; i++) {
        found = bsearch(&i, D, index, sizeof(int), compare_ints);
        
        if (!found) {
            if (row1[i] != 0 || row2[i] != 0) {
                first_nonzero = i;
                break;
            }
        }
    }

    // 如果所有元素都为 0,则认为相等
    if (first_nonzero == -1) {
        return 1;
    }

    // 计算第一个非零元素的比率
    if ((row2[first_nonzero] == 0 && row1[first_nonzero] != 0) || (row2[first_nonzero] != 0 && row1[first_nonzero] == 0)) {
        return 0; // 不相等
    }
    ratio = row1[first_nonzero] / row2[first_nonzero];

    // 比较其余需要比较的元素
    for (j = first_nonzero + 1; j < n; j++) {
        found = bsearch(&j, D, index, sizeof(int), compare_ints);
        if (!found) {
            if (row2[j] == 0 && row1[j] == 0) {
                continue;
            }
            if (fabs(row1[j] / row2[j] - ratio) > DPAR_PRESOLVE_TOL_AIJ) {
                return 0; // 不相等
            }
        }
    }

    return 1; // 相等
}



// 检查矩阵中的重复行
int CheckDuplicatedRows(double **A, int m, int n, DupRowInfo **DupRows, ColInfo **SingletonCols, int nSingletons ,int nThread) {
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
        

        int D[n];
        int index = 0;
        for (int j = 0; j < nSingletons; j++) {
            if ( (*SingletonCols)[j].type == 3) {
                D[index++] = (*SingletonCols)[j].jcol;
                //printf("单元素列号:%d ",(*SingletonCols)[j].jcol);
            }
        }
        
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (CompareRows(A[i], A[j], D, n,index)) {
                    
                    // 找到重复行
                    tempRows[rowsCount].irow = i;
                    tempRows[rowsCount].krow = j;
                    //printf("Rows %d and %d are duplicated.\n", i,j);
                    rowsCount++;
                    
                }
            }
        }
        
        // 重新分配内存以适应实际找到的重复列对数量
        *DupRows = realloc(tempRows, rowsCount * sizeof(DupRowInfo));
        if (!*DupRows) {
            free(tempRows);
            fprintf(stderr, "error2!\n");
            return -1; // 内存重新分配失败
        }

        return rowsCount; // 返回找到的重复行对的数量
    } else {
        // 多线程版本
        //printf("nthread!\n");
        
        int rowsCount = 0;
        
        DupRowInfo *tempRows = malloc(n * (n - 1) / 2 * sizeof(DupRowInfo));
        if (!tempRows) {
            fprintf(stderr, "error1!\n");
            return -1; // 内存分配失败
        }
        
        int D[n];
        int index = 0;
        for (int j = 0; j < nSingletons; j++) {
            if ( (*SingletonCols)[j].type == 3) {
                D[index++] = (*SingletonCols)[j].jcol;
                //printf("单元素列号:%d ",(*SingletonCols)[j].jcol);
            }
        }

        #pragma omp parallel num_threads(nThread)
        {
            //int tid = omp_get_thread_num();
            #pragma omp for schedule(dynamic, 1) reduction(+:rowsCount)
            for (int i = 0; i < n; ++i) {
                for (int j = i + 1; j < n; ++j) {
                    if (CompareRows(A[i], A[j], D, n,index)) {
                        #pragma omp critical
                        {
                            // 找到重复行
                            tempRows[rowsCount].irow = i;
                            tempRows[rowsCount].krow = j;
                            //printf("Rows %d and %d are duplicated.\n", i,j);
                            rowsCount++;
                        }
                    }
                }
            }
        
       }
       // 重新分配内存以适应实际找到的重复列对数量
        *DupRows = realloc(tempRows, rowsCount * sizeof(DupRowInfo));
        if (!*DupRows) {
            free(tempRows);
            fprintf(stderr, "error2!\n");
            return -1; // 内存重新分配失败
        }

        return rowsCount; // 返回找到的重复行对的数量
     }

}


int main(int argc, char *argv[]) {
    double total_time_used = 0.0;
    int iterations = 1;        //执行测试的次数
    double **A = malloc(M * sizeof(double *));
    double *B = malloc(M * sizeof(double));
    double *C = malloc(N * sizeof(double));
    double *pA = malloc(M * N * sizeof(double));
    DupRowInfo *Rows = malloc(M * sizeof(DupRowInfo));
    ColInfo *Cols = malloc(N * sizeof(ColInfo));
    int ColsSize = 0; // 用于存储空列和单元素列的数量

    if (A == NULL || pA == NULL || B == NULL || C == NULL || Rows == NULL || Cols == NULL) {
        printf("Malloc matrix failed!\n");
        free(A); free(pA); free(B); free(C); free(pA); free(Rows); free(Cols); 
        return 1;
    }
    
    // 初始化矩阵A的指针
    for (int i = 0; i < M; i++) {
        A[i] = &(pA[i * N]);

    }

    RandomMatrix(A, M, N);

    


    // 设置重复行
    for (int i = 0; i < N; i++) {
        A[1][i] = 2*A[0][i];   
        A[2][i] = A[3][i]; 

    }

    //设置单元素列
    A[0][2] = 3;
    for (int i = 1; i < M; i++) {
        A[i][2] = 0;
    }

    
    for (int j = 0; j < M; j++) {
        A[j][5] = 0;
    }
    A[2][5] = 4;



    matrix_check(A, B, C, 1, M, N, 1);

    

    CheckEmptyAndSingletonCols(C, M, N, &Cols, &ColsSize, 1);
    
    

    //单线程测试循环
    for (int iter = 0; iter < iterations; iter++) {
        clock_t start, end;
        double cpu_time_used;
        start = clock();
        
        // 检测重复行
        int duplicatedRowsCount = CheckDuplicatedRows(A, M, N, &Rows,&Cols, ColsSize, 1);
            
        end = clock();

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_time_used += cpu_time_used;
    }

    double average_time_used = total_time_used / iterations;
    printf("* M: %d ,N: %d ,单线程平均函数执行耗时: %f 秒\n", M, N, average_time_used);
    
    
    
    total_time_used = 0;
    
    //2线程测试循环
    for (int iter = 0; iter < iterations; iter++) {
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        // 检测重复行
        int duplicatedRowsCount = CheckDuplicatedRows(A, M, N, &Rows,&Cols, ColsSize, 2);
            
        end = clock();

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_time_used += cpu_time_used;
    }

    average_time_used = total_time_used / iterations;
    printf("* M: %d ,N: %d ,二线程平均函数执行耗时: %f 秒\n", M, N, average_time_used);
    
    total_time_used = 0;
    
    
    //4线程测试循环
    for (int iter = 0; iter < iterations; iter++) {
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        // 检测重复列
        int duplicatedRowsCount = CheckDuplicatedRows(A, M, N, &Rows,&Cols, ColsSize, 4);
            
        end = clock();

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_time_used += cpu_time_used;
    }

    average_time_used = total_time_used / iterations;
    printf("* M: %d ,N: %d ,四线程平均函数执行耗时: %f 秒\n", M, N, average_time_used);
    
    
    total_time_used = 0;
    
    
    //8线程测试循环
    for (int iter = 0; iter < iterations; iter++) {
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        // 检测重复行
        int duplicatedRowsCount = CheckDuplicatedRows(A, M, N, &Rows,&Cols, ColsSize, 8);
            
        end = clock();

        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_time_used += cpu_time_used;
    }

    average_time_used = total_time_used / iterations;
    printf("* M: %d ,N: %d ,八线程平均函数执行耗时: %f 秒\n", M, N, average_time_used);

    // 释放内存
    free(A);
    free(pA);
    free(B);
    free(C);
    free(Rows);
    free(Cols);

    return 0;
}
