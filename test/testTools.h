#pragma once
#include <vector>
#include <tuple>
#include <memory>

namespace MatrixTest {
using namespace std;
// filepath, matrix, matrix_m, matrix_n
typedef tuple<string, double **, int, int> fileInfo;
auto readMatrix(double** matrix, int M, int N);
/// @brief string -> C<filePath>
/// @test OK
auto getFiles(string dirPath)
-> vector<string>;

/// @brief string -> C<fileInfo>
auto readFile(string)
-> fileInfo;

/// @brief Read all data and test them in four functions.
/// string -> parse data
void testAll(string dataPath);

}
