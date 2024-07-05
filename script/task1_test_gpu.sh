echo "开始GPU执行task1_gpu_test_gpu,结果将被写入到output/task1_gpu.md"
echo "# GPU of Task 1 Output" >> output/task1_gpu.md
./bin/task_1_gpu "./data/A(100x200).bin" 100 200 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/A(700x1500).bin" 700 1500 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/A(821x1571).25fv47.bin" 821 1571 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/A(1000x2000).bin" 1000 2000 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "../A(2262x9799).80bau3b.bin" 2262 9799 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/AA(821x2271).25fv47.bin" 821 2271 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "../AA(2262x11099).80bau3b.bin" 2262 11099 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/S(100x100).bin" 100 100 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/S(700x700).bin" 700 700 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/S(821x821).25fv47.bin" 821 821 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/S(1000x1000).bin" 1000 1000 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/S(2262x2262).80bau3b.bin" 2262 2262 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/T(100x100).bin" 100 100 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/T(700x700).bin" 700 700 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/T(1000x1000).bin" 1000 1000 >> output/task1_gpu.md 2>&1
./bin/task_1_gpu "./data/A(821x1571).25fv47.bin" 821 1571 >> output/task1_gpu.md 2>&1

echo "task1_gpu_test执行完毕"