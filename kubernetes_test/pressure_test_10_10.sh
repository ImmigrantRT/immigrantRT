#!/bin/bash
mkdir log
rm log/pressure_test_10_10.log
kubectl delete deployments --all
while [ $(kubectl get pods --no-headers | wc -l) -ne 0 ]; do
	sleep 1
done
kubectl apply -f deployment_10.yaml
for i in {1..10}
do
	while [ $(kubectl get pods --field-selector=status.phase=Running --no-headers | wc -l) -ne 10 ]; do
		sleep 1
	done
	TIMEFORMAT=$'%R s'
	{ time kubectl delete pods --all ;} 2>> log/pressure_test_10_10.log
done
