#!/bin/bash
mkdir log
rm log/pressure_test_10_100.log
kubectl delete deployments --all
while [ $(kubectl get pods --no-headers | wc -l) -ne 0 ]; do
        sleep 1
done
kubectl apply -f deployment_1000.yaml
for i in {1..10}
do
	while [ $(kubectl get pods --field-selector=status.phase=Running --no-headers | wc -l) -ne 1000 ]; do
                sleep 1
        done
	TIMEFORMAT=$'%R s'
	{ time kubectl delete pod $(kubectl get pods --no-headers -o custom-columns=":metadata.name" | shuf -n 10) ;} 2>> log/pressure_test_10_1000.log
done
