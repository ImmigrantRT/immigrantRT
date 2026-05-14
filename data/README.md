### Folder Structure

```
.
├── ...
|
├──	data
|	|
|	├── containers
|	|	├── [container id]
|	|	|	├── rank.txt
|	|	|	├── requirements.txt
|	|	|	├── stats_processed.csv
|	|	|	└── stats.txt
|	|	...
|	|
|	├── remote_nodes
|	|	├── [ip address]
|	|	|	├── forecasts.csv
|	|	|	├── health.txt
|	|	|	├── lines.txt
|	|	|	├── stats_processed.csv
|	|	|	├── stats_tail.csv
|	|	|	├── stats.txt
|	|	|	└── throughput.txt
|	|	...
|	|
|	├── self
|	|	├── forecasts.csv
|	|	├── stats_processed.csv
|	|	├── stats_tail.csv
|	|	└── stats.txt
|	|
|	├── containers.txt
|	├── nodes.txt                  
|	├── ports.pkl
|	└── README.md
|
└── ...
```


### Subfolders

* `containers/` 
	- separate folder `[container id]/` for each container
	- contains the data collected for the containers running on the current node
	
* `remote_nodes/` 
	- separate folder `[ip address]/` for each remote node
	- contains the data collected from remote nodes, node health information, throughput between the current node and the remote node and the ARIMA forecasts

* `self/` 
	- contains the resource availability history of current node and the ARIMA forecasts


### File Formats

* `nodes.txt`
	- list of known nodes in the network
	- format:

		```
		[ip address 1]
		[ip address 2]
		...
		...
		[ip address N]
		```

* `ports.pkl`
	- generated during the setup phase
	- contains a `pandas` dataframe with the ports to be used for throughput measurement
	- dataframe format: for example,
		- consider a system of 3 nodes n1, n2, n3 with IP addresses ip1, ip2, ip3 respectively
		- a block of unreserved ports (starting at port `p`) is used for throughput measurement

			|     | ip1 | ip2 | ip3 |
			| --- | --- | --- | --- |
			| **ip1** |  0  | p | p+1 |
			| **ip2** | p+2 |  0  | p+3 |
			| **ip3** | p+4 | p+5 |  0  |
		
		- node n1:
			- runs 2 copies of the `iperf3` server listening on ports p and p+1 respectively.
			- runs 2 copies of the client writing to ip2:p+2 and ip3:p+4.

* `containers.csv`
	- list of containers running on the current node and their criticality levels
	- format:
	
		```
		[container id 1],[crit level 1]
		[container id 2],[crit level 2]
		...
		...
		[container id C],[crit level C]
		```

* `stats.txt`
	- raw data recorded by the monitor
	- format: each line has
		- timestamp, % CPU available, memory (in kB) available for nodes 

			```
			[11/11/2023 17:42:57.373157240] 17.042606516290732,29571896
			[11/11/2023 17:42:58.379123509] 24.875621890547265,29570696
			``` 
			
		- timestamp, % CPU needed, memory (in kB) needed for containers

* `stats_processed.csv`
	- data after processing
	- format: each timestamp in `stats.txt` is replaced with the time elapsed since the start of the observation

		```
		1.000000,17.042607,29571896.000000
		2.000000,24.875622,29570696.000000
		```

* `requirements.txt`
	- contains the resources needed by the container on every every node in the system
	- format: each line has the node IP address, CPU (%) required, memory needed (in KB).
	
		```
		[ip_address_1],10,150000
		[ip_address_2],12,120000
		[ip_address_3],18,200000
		```

* `health.txt`
	- contains the history of whether the node was online/ offline
	- format: (1 = online, 0 = offline)

		```
		[03/17/2022 16:24:49.540586] 0
		[03/17/2022 16:24:50.043436] 1
		[03/17/2022 16:24:50.560790] 1
		```

* `throughput.txt`
	- contains the throughput in MBps

* `rank.txt`
	- ranked list of `M` candidate nodes (top = preferred) for the container
	- format: similar to `nodes.txt`
