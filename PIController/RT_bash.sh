#!/bin/bash
KpU=0
KpM=0
rm -f reports/*
for i in {10..20}
do
    for j in {10..20}
    do
	    KpU=$(echo "scale=2 ; $i / 100" | bc)
	    KpM=$(echo "scale=2 ; $j / 100" | bc)
        echo Kp_U=$KpU Kp_M=$KpM
        ./RT_test_scipt.sh $KpU $KpM
        sleep 10
	mv log/report.csv reports/report_${KpU}_${KpM}.csv
	sleep 5
    done
done
