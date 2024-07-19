#!/bin/bash

echo "运行"

./task_master -h

./task_master helen 114514 addtask task1 low life 2024 7 8 11 50 2024 7 8 11 45
./task_master helen 114514 addtask task2 middle play 2024 7 8 12 30 2024 7 8 12 25
./task_master helen 114514 addtask task3 high work 2024 7 8 13 15 2024 7 8 13 10
./task_master helen 114514 addtask task4 emergent study 2024 7 8 14 00 2024 7 8 13 55
./task_master helen 114514 addtask task5 low life 2024 7 8 14 45 2024 7 8 14 40
./task_master helen 114514 addtask task6 middle play 2024 7 8 15 30 2024 7 8 15 25
./task_master helen 114514 addtask task7 high work 2024 7 8 16 15 2024 7 8 16 10
./task_master helen 114514 addtask task8 emergent study 2024 7 8 17 00 2024 7 8 16 55
./task_master helen 114514 addtask task9 low life 2024 7 8 17 45 2024 7 8 17 40
./task_master helen 114514 addtask task10 middle play 2024 7 8 18 30 2024 7 8 18 25
./task_master helen 114514 addtask task11 high work 2024 7 8 19 15 2024 7 8 19 10
./task_master helen 114514 addtask task12 emergent study 2024 7 8 20 00 2024 7 8 19 55
./task_master helen 114514 addtask task13 low life 2024 7 8 20 45 2024 7 8 20 40
./task_master helen 114514 addtask task14 middle play 2024 7 8 21 30 2024 7 8 21 25
./task_master helen 114514 addtask task15 high work 2024 7 8 22 15 2024 7 8 22 10
./task_master helen 114514 addtask task16 emergent study 2024 7 8 23 00 2024 7 8 22 55
./task_master helen 114514 addtask task17 low life 2024 7 9 09 45 2024 7 9 09 40
./task_master helen 114514 addtask task18 middle play 2024 7 9 10 30 2024 7 9 10 25
./task_master helen 114514 addtask task19 high work 2024 7 9 11 15 2024 7 9 11 10
./task_master helen 114514 addtask task20 emergent study 2024 7 9 12 00 2024 7 9 11 55
./task_master helen 114514 addtask task21 low life 2024 7 9 12 45 2024 7 9 12 40
./task_master helen 114514 addtask task22 middle play 2024 7 9 13 30 2024 7 9 13 25
./task_master helen 114514 addtask task23 high work 2024 7 9 14 15 2024 7 9 14 10
./task_master helen 114514 addtask task24 emergent study 2024 7 9 15 00 2024 7 9 14 55
./task_master helen 114514 addtask task25 low life 2024 7 9 15 45 2024 7 9 15 40
./task_master helen 114514 addtask task26 middle play 2024 7 9 16 30 2024 7 9 16 25
./task_master helen 114514 addtask task27 high work 2024 7 9 17 15 2024 7 9 17 10
./task_master helen 114514 addtask task28 emergent study 2024 7 9 18 00 2024 7 9 17 55
./task_master helen 114514 addtask task29 low life 2024 7 9 18 45 2024 7 9 18 40
./task_master helen 114514 addtask task30 middle play 2024 7 9 19 30 2024 7 9 19 25
./task_master helen 114514 addtask task31 high work 2024 7 9 20 15 2024 7 9 20 10
./task_master helen 114514 addtask task32 emergent study 2024 7 9 21 00 2024 7 9 20 55
./task_master helen 114514 addtask task33 low life 2024 7 9 21 45 2024 7 9 21 40
./task_master helen 114514 addtask task34 middle play 2024 7 9 22 30 2024 7 9 22 25
./task_master helen 114514 addtask task35 high work 2024 7 9 23 15 2024 7 9 23 10
./task_master helen 114514 addtask task36 emergent study 2024 7 10 09 00 2024 7 10 08 55
./task_master helen 114514 addtask task37 low life 2024 7 10 09 45 2024 7 10 09 40
./task_master helen 114514 addtask task38 middle play 2024 7 10 10 30 2024 7 10 10 25
./task_master helen 114514 addtask task39 high work 2024 7 10 11 15 2024 7 10 11 10
./task_master helen 114514 addtask task40 emergent study 2024 7 10 12 00 2024 7 10 11 55

./task_master helen 114514 deltask task40
./task_master helen 114514 deltask task30
./task_master helen 114514 deltask task20
./task_master helen 114514 deltask task10

./task_master helen 114514 showtask 2
./task_master helen 114514 showtask 8
./task_master helen 114514 showtask 2024 7 9
./task_master helen 114514 showtask Mon
./task_master helen 114514 showtask

./task_master run