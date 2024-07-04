#include "testTools.h"
#include <filesystem>
#include <string>
#include <ranges>
#include <regex>
#include <format>
#include <iostream>
#include <fstream>

namespace MatrixTest {

using namespace std;

auto getFiles(string dirPath)
-> vector<string> {
    vector<string> files;

    // filter: not directory
    auto filterFunction = [](auto ff) -> bool {
        return filesystem::is_regular_file(ff);
    };

    // open dir
    filesystem::path fileBasePath(dirPath);
    for (const auto & file : filesystem::directory_iterator(fileBasePath)
                             | views::filter(filterFunction)) {
        files.push_back(file.path());
    }

    return files;
}

auto readFile(string filePath)
-> fileInfo {
    fileInfo fInfo;

    regex MxN = regex("\\((\\d+)x(\\d+)\\)");
    smatch match;

    if (regex_search(filePath, match, MxN)) {
        get<2>(fInfo) = stoi(match[1].str());
        get<3>(fInfo) = stoi(match[2].str());
    }

    int matrix_rows = get<2>(fInfo);
    int matrix_cols = get<3>(fInfo);

    ifstream fin(filePath, ios::binary);

    if (!fin.is_open()) {
        cerr << std::format("Open file wrong! {}", filePath) << endl;         
    }

    auto matrix = new double*[matrix_rows];
    double* content = new double[matrix_rows * matrix_cols];
    fin.read((char* )content, sizeof(double) * matrix_rows * matrix_cols);

    for (int count = 0; count < matrix_rows; count++) {
        matrix[count] = content + (matrix_cols * count);
    }

    get<1>(fInfo) = matrix;
    get<0>(fInfo) = filePath;

    fin.close();
    return fInfo;
}

auto readMatrix(fileInfo fInfo) {
    double** matrix = get<1>(fInfo);
    cout << format("{} {} {}", get<0>(fInfo), get<2>(fInfo), get<3>(fInfo)) << endl;
    for (int i = 0; i < get<2>(fInfo); ++i) {
        for (int j = 0; j < get<3>(fInfo); ++j) {
            if (matrix[i][j]) {
                cout << format("{} {} {}", i, j, matrix[i][j]) << endl;
            }
        }
    }
}

auto changeMatrix(fileInfo fInfo) -> fileInfo {


    return fInfo;
}

void testAll(string dataPath) {
    for (const auto & fileInfo : getFiles(dataPath) 
                                 | ranges::views::transform(readFile)) {
        auto MatrixName = get<0>(fileInfo);
        auto Matrix = get<1>(fileInfo);
        auto Matrix_M = get<2>(fileInfo);
        auto Matrix_N = get<3>(fileInfo);

        //运行程序

        changeMatrix(fileInfo);

        //再次运行程序

        //比较两次结果

    }
}

}
