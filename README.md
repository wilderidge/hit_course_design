# 哈工大计科课程设计 第一组
## 任务一
* [x] 空行和单元素行的检测
* [x] 空列和单元素列的检测
## 任务二
* [x] 重复列检测
## 任务三
* [x] 重复行检测
## 任务四
* [x] 生成或读取矩阵数据
* [x] 测试所有函数的正确性
* [x] 测试所有函数的性能
## 性能提升
### 多线程改造
* [x] 任务一的多线程改造
* [x] 任务二的多线程改造
* [x] 任务三的多线程改造
### cuda的引入
* [x] 任务一的cuda引入
* [ ] 任务二的cuda引入
* [ ] 任务三的cuda引入
### 循坏展开
* [x] 任务一循环展开
* [ ] 任务二循环展开
* [ ] 任务三循环展开

## Use it 
安装cmake
```
cmake -B build
cmake --build build
```
可执行文件在bin文件夹中

## GPU编译
nvcc task_1_gpu.cu ./public/Utils.o -o ./bin/task_1_gpu
编译时需要链接接Utils.o库否则编译出错