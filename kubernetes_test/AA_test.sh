#!/bin/bash
mkdir log
rm log/AA.log
kubectl delete deployments --all
while [ $(kubectl get pods --no-headers | wc -l) -ne 0 ]; do
	sleep 1
done
kubectl apply -f deployment_AA.yaml
for i in {1..10}
do
	while [ $(kubectl get pods --field-selector=status.phase=Running --no-headers | wc -l) -ne 1 ]; do
		sleep 1
	done
	TIMEFORMAT=$'%R s'
	{ time kubectl delete pods --all ;} 2>> log/AA.log
done
