#!/bin/bash

# path to different folders
script_dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")
dirs=("data" "misc" "monitor" "prediction" "analysis" "controller" "migration")
for dir in "${dirs[@]}"; do
	eval "${dir}_dir=\$script_dir/../${dir/controller/PIController}"
done

# read values of parameters from config.yaml
config () { echo "$(python3 $misc_dir/parse_config.py $1)" ; }

username=$(config 'user')

do_control=$(config 'do_control')
cpuaffinity=$(config 'cpuaffinity')
cpuaffinity_folder=$(config 'cpuaffinity_folder')
do_migration=$(config 'do_migration')

t_timeout=$(config 'timeout')

t_repeat_ping=$(echo "scale=2; 1 / $(config 'monitor.ping.frequency')" | bc)
t_repeat_throuphput=$(echo "scale=2; 1 / $(config 'monitor.throughput.frequency')" | bc)
t_sleep_collection=$(config 'monitor.data_collection.sleep')
t_repeat_collection=$(echo "scale=2; 1 / $(config 'monitor.data_collection.frequency')" | bc)

t_sleep_aggregation=$(config 'misc.aggregation.sleep')
t_repeat_aggregation=$(echo "scale=2; 1 / $(config 'misc.aggregation.frequency')" | bc)

t_sleep_arima=$(config 'prediction.arima.sleep')
t_repeat_arima=$(echo "scale=2; 1 / $(config 'prediction.arima.frequency')" | bc)
n_forecasts_arima=$(config 'prediction.arima.forecasts')
n_samples_curve_fitting=$(config 'prediction.curve_fitting.samples')
t_pred_curve_fitting=$(config 'prediction.curve_fitting.prediction_time')

t_sleep_ranking=$(config 'analysis.ranking.sleep')
t_repeat_ranking=$(echo "scale=2; 1 / $(config 'analysis.ranking.frequency')" | bc)
t_repeat_trigger=$(echo "scale=2; 1 / $(config 'analysis.trigger.frequency')" | bc)

t_sleep_migration_check=$(config 'migration.sleep')
t_sleep_pre_checkpoint=$(config 'migration.pre_checkpoint.sleep')
t_repeat_pre_checkpoint=$(echo "scale=2; 1 / $(config 'migration.pre_checkpoint.frequency')" | bc)

current_ip=$(hostname -I | awk '{print $1}')
total_mem=$(cat /proc/meminfo | grep MemTotal | awk '{print $2}')

t_reference=$(date +"[%m/%d/%Y %H:%M:%S.%N]")

start_node_monitor () {
	timeout $t_timeout python3 $monitor_dir/node_monitor.py $cpuaffinity &
}

start_container_monitor () {
	while IFS= read -r info && [ -n "$info" ]
	do
		IFS=',' read -r -a array <<< "$info"
		sudo timeout $t_timeout $monitor_dir/container_monitor/bin/exec $data_dir ${array[0]} $cpuaffinity &
	done < "$data_dir/containers.csv"
}

start_aggregator () {
	sleep $t_sleep_aggregation
	local t_remaining=$(echo "$t_timeout - $t_sleep_aggregation" | bc)

	commands=(
		"python3 $misc_dir/aggregation.py '$t_reference' $total_mem"
	)
	timeout $t_remaining $misc_dir/repeat/bin/exec $t_repeat_aggregation "${commands[@]}" &
}

start_controller () {
	#export RTCdir=$PWD
	#sudo $controller_dir/src/gcp_with_monitor.sh $cpuaffinity
	rm -f $controller_dir/log/error_msg.txt
	#sudo dmesg -c &
	sudo $controller_dir/bin/PIController $t_timeout $do_control &
}

ping_remote_nodes () {
	while IFS= read -r ip && [ -n "$ip" ]
	do
		commands=(
			"bash $monitor_dir/ping.sh $ip"
		)
		timeout $t_timeout $misc_dir/repeat/bin/exec $t_repeat_ping "${commands[@]}" &
	done < "$data_dir/nodes.txt"
}

measure_throughput () {
	while IFS= read -r ip && [ -n "$ip" ]
	do
		commands=(
			"python3 $monitor_dir/throughput/client.py $ip"
		)
		timeout $t_timeout $misc_dir/repeat/bin/exec $t_repeat_throuphput "${commands[@]}" &
	done < "$data_dir/nodes.txt"
}

collect_stats_from_remote_nodes_and_process() {
	sleep $t_sleep_collection
	local t_remaining=$(echo "$t_timeout - $t_sleep_collection" | bc)

	while IFS= read -r ip && [ -n "$ip" ]
	do
		dir=$data_dir/remote_nodes/$ip
		commands=(
			"bash $monitor_dir/data_collection.sh $ip $username"
			"grep -c '.' $dir/stats.txt > $dir/lines.txt"
			"$misc_dir/processing/bin/exec $dir '$t_reference'"
		)
		timeout $t_remaining $misc_dir/repeat/bin/exec $t_repeat_collection "${commands[@]}" &
	done < "$data_dir/nodes.txt"
}

run_arima_forecasting () {
	sleep $t_sleep_arima
	local t_remaining=$(echo "$t_timeout - $t_sleep_arima" | bc)

	while IFS= read -r ip && [ -n "$ip" ]
	do
		commands=(
			"python3 $prediction_dir/arima/main.py $data_dir/remote_nodes/$ip $n_forecasts_arima"
		)
		timeout $t_remaining $misc_dir/repeat/bin/exec $t_repeat_arima "${commands[@]}" &
	done < "$data_dir/nodes.txt"
}

rank_remote_nodes () {
	sleep $t_sleep_ranking	# start after 1 iteration of forecasting
	local t_remaining=$(echo "$t_timeout - $t_sleep_ranking" | bc)

	commands=(
		"$analysis_dir/ranking/bin/exec $data_dir"
	)
	timeout $t_remaining $misc_dir/repeat/bin/exec $t_repeat_ranking "${commands[@]}" &
}

pre_checkpoint_all_containers () {
	sleep $t_sleep_pre_checkpoint	# start after 10 seconds
	local t_remaining=$(echo "$t_timeout - $t_sleep_pre_checkpoint" | bc)

	while IFS= read -r id && [ -n "$id" ]
	do
		IFS=', ' read -r -a array <<< "$id"
		commands=(
			"python3 $migration_dir/client.py 'r' ${array[0]}"
		)
		timeout $t_remaining $misc_dir/repeat/bin/exec $t_repeat_pre_checkpoint "${commands[@]}" &
	done < "$data_dir/containers.csv"
}

run_curve_fitting () {
	if [ $# -eq 1 ]; then
		dir=$data_dir/remote_nodes/$1
	else
		dir=$data_dir/other
	fi

	tail -n $n_samples_curve_fitting $dir/stats_processed.csv > $dir/stats_tail.csv
	echo $($prediction_dir/curve_fitting/bin/exec $dir $t_pred_curve_fitting)
}

validate_candidates () {
	read_preds=0

	while IFS= read -r ip && [ -n "$ip" ]
	do
		dir=$data_dir/remote_nodes/$ip

		num_lines_stats=$(grep -c '.' $dir/stats.txt)
		num_lines_processed=$(<$dir/lines.txt)

		if [ $((num_lines_stats - num_lines_processed)) -gt 0 ]; then
			$misc_dir/processing/bin/exec $dir "$t_reference"
			run_curve_fitting $ip
			read_preds=1
		fi
	done < "$data_dir/containers/$1/rank.txt"

	$analysis_dir/validation/bin/exec $data_dir $1 $read_preds
}

container_select () {
	run_curve_fitting

	container_selected=$($analysis_dir/trigger/bin/exec $data_dir $current_ip)
	echo $container_selected
}

migration_decision () {
	t_start=`date +%s.%N`

	while true
	do
		t1=`date +%s.%N`

		#run_curve_fitting

		#container_selected=$($analysis_dir/trigger/bin/exec $data_dir $current_ip)
		container_select

		if [ -n "$container_selected" ]; then
			validate_candidates "$container_selected"

			echo 1=$data_dir 2=$current_ip res=$container_selected >> $data_dir/migration.log
			break
		fi

		t2=`date +%s.%N`
		t=$(echo $t_repeat_trigger - $( echo "$t2 - $t1" | bc -l ) | bc)
		
		if [[ $t != -* ]]; then	# t is positive
			sleep $t
		fi
	done

	t_end=`date +%s.%N`

	# migrate container
	python3 $migration_dir/client.py "m" "$1"

	echo $(echo "$t_end - $t_start" | bc) $1 >> $data_dir/migration.log
	echo "" > $data_dir/self/QoS.csv
}

sudo -v # ask for password up front

# echo $(date +%s.%N) > $data_dir/migration.log
echo "" > $data_dir/migration.log

start_controller
start_node_monitor
start_aggregator
start_container_monitor
ping_remote_nodes
measure_throughput
collect_stats_from_remote_nodes_and_process &
run_arima_forecasting &
rank_remote_nodes &
#pre_checkpoint_all_containers &

sleep $t_sleep_migration_check 	# after 1 iteration of forecasting + ranking
t_remaining=$(echo "$t_timeout - $t_sleep_migration_check" | bc)

#timeout $t_remaining migration_decision

echo $t_reference >> $data_dir/experiment_time.txt
