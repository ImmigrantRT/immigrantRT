#!/bin/bash
rm log/*
dmesg -c
RT=50
CPU=$(nproc --all)
while [ ${RT} -gt 0 ]
do
    count=0
    for (( i=1; i<=$((CPU*100/RT)); i++ ))
    do
        count=$((count+1))
        /home/briankao/RT-Linux-Task-Tester/mix 120 1 100 ${RT} >> log/p${count}.log &
    done
    sleep 0.1
    rm task_list.txt
    pgrep mix >> task_list.txt
    obj/PIController 100 0.17 0.1
    mv log/report.csv reports/scaleup/report_RT${RT}_${count}.csv
    sleep 120
    RT=$((RT/2))
done
