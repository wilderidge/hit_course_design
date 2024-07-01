#!/bin/bash

# 编译 task_1 和 task_2
gcc task_1.c public/Utils.c -o bin/task_1
gcc task_2.c public/Utils.c -o bin/task_2

# 编译测试
g++ test/test.cc -o bin/test -lgtest