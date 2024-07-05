echo "开始CPU执行task2_test,结果将被写入到output/task2.md"
echo "# CPU of Task 1 Output" > output/task2.md
./bin/task_2 "./data/A(100x200).bin" 100 200 >> output/task2.md 2>&1
./bin/task_2 "./data/A(700x1500).bin" 700 1500 >> output/task2.md 2>&1
./bin/task_2 "./data/A(821x1571).25fv47.bin" 821 1571 >> output/task2.md 2>&1
./bin/task_2 "./data/A(1000x2000).bin" 1000 2000 >> output/task2.md 2>&1
./bin/task_2 "../A(2262x9799).80bau3b.bin" 2262 9799 >> output/task2.md 2>&1
./bin/task_2 "./data/AA(821x2271).25fv47.bin" 821 2271 >> output/task2.md 2>&1
./bin/task_2 "../AA(2262x11099).80bau3b.bin" 2262 11099 >> output/task2.md 2>&1
./bin/task_2 "./data/S(100x100).bin" 100 100 >> output/task2.md 2>&1
./bin/task_2 "./data/S(700x700).bin" 700 700 >> output/task2.md 2>&1
./bin/task_2 "./data/S(821x821).25fv47.bin" 821 821 >> output/task2.md 2>&1
./bin/task_2 "./data/S(1000x1000).bin" 1000 1000 >> output/task2.md 2>&1
./bin/task_2 "./data/S(2262x2262).80bau3b.bin" 2262 2262 >> output/task2.md 2>&1
./bin/task_2 "./data/T(100x100).bin" 100 100 >> output/task2.md 2>&1
./bin/task_2 "./data/T(700x700).bin" 700 700 >> output/task2.md 2>&1
./bin/task_2 "./data/T(1000x1000).bin" 1000 1000 >> output/task2.md 2>&1
./bin/task_2 "./data/A(821x1571).25fv47.bin" 821 1571 >> output/task2.md 2>&1

echo "task2_test执行完毕"