#!/bin/bash
#sudo rm task_list.txt
sudo rm log/*
sudo dmesg -c
#sudo ~/RT-Linux-Task-Tester/mix 50 1 100 90 >> log/p3.log &
echo create R1
#sudo taskset -c 0 sudo ../RT-Linux-Task-Tester/mix 50 1 100 40 >> log/p1.log &
sudo /mydata/RT-Linux-Task-Tester/mix 50 1 50 20 >> log/p1.log &
echo create R2
sudo /mydata/RT-Linux-Task-Tester/mix 50 1 50 10 >> log/p2.log &
echo create R3
sudo /mydata/RT-Linux-Task-Tester/mix 50 1 50 10 >> log/p3.log &
echo create R4
sudo /mydata/RT-Linux-Task-Tester/mix 50 1 50 10 >> log/p4.log &
echo create R5
sudo /mydata/RT-Linux-Task-Tester/mix 50 1 100 80 >> log/p5.log &
#echo create R6
#sudo ~/RT-Linux-Task-Tester/mix 50 1 100 60 >> log/p6.log &
#echo create R7
#sudo ~/RT-Linux-Task-Tester/mix 50 1 100 40 >> log/p7.log &
#echo create R8
#sudo ~/RT-Linux-Task-Tester/mix 50 1 100 5 >> log/p8.log &
#sudo ~/RT-Linux-Task-Tester/mix 50 1 100 20 >> log/p2.log &
#sudo ~/RT-Linux-Task-Tester/mix 50 1 100 20 >> log/p2.log &
#sudo ~/RT-Linux-Task-Tester/mix 50 1 100 20 >> log/p2.log &
#sudo ~/RT-Linux-Task-Tester/mix 50 1 100 20 >> log/p2.log &
#sudo ~/RT-Linux-Task-Tester/mix 50 1 100 20 >> log/p2.log &
#sudo ~/RT-Linux-Task-Tester/mix 50 1 100 20 >> log/p2.log &
#sudo ~/RT-Linux-Task-Tester/mix 50 1 100 20 >> log/p2.log &
#sudo ~/RT-Linux-Task-Tester/mix 50 1 100 20 >> log/p2.log &
sleep 0.1
pgrep mix >> task_list.txt
i=0x00000001
#while IFS= read -r line; do
#    sudo taskset -p $i $line
#    ((i=i+1))
#done < task_list.txt
echo $1 $2
#sudo obj/PIController 50 ${1} ${2}
