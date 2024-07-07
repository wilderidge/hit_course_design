#include <cuda_runtime.h>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include "public/CheckMatrix.h"
#include "public/Utils.h"

// CUDA 内核函数：检查空行和单元素行
__global__ void CheckRowsKernel(double *B, int m, int n, RowInfo *rows, int *rowCount) {
    int row = blockIdx.x * blockDim.x + threadIdx.x;
    if (row >= m) return;

    bool isEmpty = true;
    bool isSingleton = false;
    int nonZeroCount = 0;
    int nonZeroCol = -1;

    for (int j = 0; j < n; ++j) {
        if (B[row * n + j] != 0) {
            isEmpty = false;
            nonZeroCount++;
            nonZeroCol = j;
        }
    }

    if (isEmpty || nonZeroCount == 1) {
        int idx = atomicAdd(rowCount, 1);
        rows[idx].type = isEmpty ? 0 : 1;
        rows[idx].irow = row;
        rows[idx].jcol = nonZeroCol;
    }
}

// CUDA 内核函数：检查空列和单元素列
__global__ void CheckColsKernel(double *C, int m, int n, ColInfo *cols, int *colCount) {
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (col >= n) return;

    bool isEmpty = true;
    bool isSingleton = false;
    int nonZeroCount = 0;
    int nonZeroRow = -1;

    for (int i = 0; i < m; ++i) {
        if (C[i * n + col] != 0) {
            isEmpty = false;
            nonZeroCount++;
            nonZeroRow = i;
        }
    }

    if (isEmpty || nonZeroCount == 1) {
        int idx = atomicAdd(colCount, 1);
        cols[idx].type = isEmpty ? 0 : 1;
        cols[idx].jcol = col;
        cols[idx].irow = nonZeroRow;
    }
}

// 检查空行和单元素行
int CheckEmptyAndSingletonRows(double *B, int m, int n, RowInfo **Rows, int *RowsSize) {
    double *d_B;
    RowInfo *d_rows;
    int *d_rowCount;
    int rowCount = 0;

    cudaMalloc((void**)&d_B, m * n * sizeof(double));
    cudaMalloc((void**)&d_rows, m * sizeof(RowInfo));
    cudaMalloc((void**)&d_rowCount, sizeof(int));

    cudaMemcpy(d_B, B, m * n * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(d_rowCount, &rowCount, sizeof(int), cudaMemcpyHostToDevice);

    int blockSize = 256;
    int numBlocks = (m + blockSize - 1) / blockSize;

    CheckRowsKernel<<<numBlocks, blockSize>>>(d_B, m, n, d_rows, d_rowCount);

    cudaMemcpy(&rowCount, d_rowCount, sizeof(int), cudaMemcpyDeviceToHost);
    RowInfo *h_rows = (RowInfo*)malloc(rowCount * sizeof(RowInfo));
    cudaMemcpy(h_rows, d_rows, rowCount * sizeof(RowInfo), cudaMemcpyDeviceToHost);

    *Rows = h_rows;
    *RowsSize = rowCount;

    cudaFree(d_B);
    cudaFree(d_rows);
    cudaFree(d_rowCount);

    return rowCount;
}

// 检查空列和单元素列
int CheckEmptyAndSingletonCols(double *C, int m, int n, ColInfo **Cols, int *ColsSize) {
    double *d_C;
    ColInfo *d_cols;
    int *d_colCount;
    int colCount = 0;

    cudaMalloc((void**)&d_C, m * n * sizeof(double));
    cudaMalloc((void**)&d_cols, n * sizeof(ColInfo));
    cudaMalloc((void**)&d_colCount, sizeof(int));

    cudaMemcpy(d_C, C, m * n * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(d_colCount, &colCount, sizeof(int), cudaMemcpyHostToDevice);

    int blockSize = 256;
    int numBlocks = (n + blockSize - 1) / blockSize;

    CheckColsKernel<<<numBlocks, blockSize>>>(d_C, m, n, d_cols, d_colCount);

    cudaMemcpy(&colCount, d_colCount, sizeof(int), cudaMemcpyDeviceToHost);
    ColInfo *h_cols = (ColInfo*)malloc(colCount * sizeof(ColInfo));
    cudaMemcpy(h_cols, d_cols, colCount * sizeof(ColInfo), cudaMemcpyDeviceToHost);

    *Cols = h_cols;
    *ColsSize = colCount;

    cudaFree(d_C);
    cudaFree(d_cols);
    cudaFree(d_colCount);

    return colCount;
}


int main(int argc, char *argv[]) {
    char *filename = "../A(2262x9799).80bau3b.bin";
    int M = 2262;
    int N = 9799;
    if (argc == 4) {
        filename = argv[1];
        M = atoi(argv[2]);
        N = atoi(argv[3]);
    } else if (argc != 4 && argc != 1) {
        return 0;
    }

    int nThread = 8; // 线程数
    double total_time_used = 0.0;
    int iterations = 500; // 执行测试的次数

    // 动态分配内存
    double **pA = (double **)malloc(M * sizeof(double *));
    double *A = (double *)malloc(M * N * sizeof(double));
    double *B = (double *)malloc(M * sizeof(double));
    double *C = (double *)malloc(N * sizeof(double));
    RowInfo *Rows;
    ColInfo *Cols;
    int RowsSize = 0; // 用于存储空行和单元素行的数量
    int ColsSize = 0; // 用于存储空列和单元素列的数量
    for (int i = 0; i < M; i++) {
        pA[i] = &(A[i * N]);
    }
    // 从文件读取矩阵数据
    ReadMatrix(pA, M, N,filename);

    // 执行测试并计时
    for (int iter = 0; iter < iterations; ++iter) {
        auto start = std::chrono::high_resolution_clock::now();

        RowsSize = 0;
        ColsSize = 0;
        CheckEmptyAndSingletonRows(A, M, N, &Rows, &RowsSize);
        CheckEmptyAndSingletonCols(A, M, N, &Cols, &ColsSize);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        total_time_used += elapsed.count();
        // // 输出检测结果
        // if (iter == 0) { // 仅在第一次迭代输出检测结果
        //     std::cout << "Empty and singleton rows:\n";
        //     for (int i = 0; i < RowsSize; ++i) {
        //         std::cout << "Row " << Rows[i].irow << ", Type: " << (Rows[i].type == 0 ? "Empty" : "Singleton") << "\n";
        //     }
        //     std::cout << "Empty and singleton cols:\n";
        //     for (int i = 0; i < ColsSize; ++i) {
        //         std::cout << "Col " << Cols[i].jcol << ", Type: " << (Cols[i].type == 0 ? "Empty" : "Singleton") << "\n";
        //     }
        // }
    }

    double average_time_used = total_time_used / iterations;
    printf("M: %d, N: %d, GPU多线程平均函数执行耗时: %f 秒\n", M, N, average_time_used);

    // 释放内存
    free(A);
    free(B);
    free(C);
    free(Rows);
    free(Cols);

    return 0;
}
