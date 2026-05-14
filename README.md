# RT Container Framework

A framework help to manage Real-Time Container in a distributed IoT system.


## In this repo

- `PIController/`: source code for the controller
- `analysis/`: source code for the analysis engine
- `apps/`: applications for testing
- `data/`: contains
	- user-provided information about the known nodes (and ports)
	- raw and processed data collected during the execution
- `experiments/`: code for all the experiments
- `launch/`: contains
	- module to repeat system commands periodically
	- yaml file with configurable parameters
	- script to launch the iperf3 and migration servers
	- script to run the whole framework
- `migration/`: code for carrying out the migration
- `monitor/`: source code for monitoring nodes and containers
- `prediction/`: source code for the prediction models
- `ubuntu_kernel_5.15.71-rt51/ : costomized ubuntu RT kernel which write the miss deadline message to dmesg
- `setup/`: contains
	- script to install packages
	- pip requirements file
	- script to create folders and files
	- script to build executables from C, C++ source code
	- script to delete object files and executables


## Running the framework

Run all of the following commands from the main folder.

### On all nodes:

- Upgrade ubuntu to 22.04
	```
	sudo apt update -y && sudo apt full-upgrade -y
	sudo reboot
	```
	```
	sudo do-release-upgrade
	```

- Install costomized ubuntu kernel
	```
	cd ubuntu_kernel_5.15.71-rt51
	cat linux-image-5.15.71-rt51_5.15.71-rt51-1_amd64* > linux-image-5.15.71-rt51_5.15.71-rt51-1_amd64.deb
	cat linux-image-5.15.71-rt51-dbg_5.15.71-rt51-1_amd64* > linux-image-5.15.71-rt51-dbg_5.15.71-rt51-1_amd64.deb
	sudo dpkg -i linux*5.15.71-rt51*.deb
	sudo reboot
	```

- Update the crun if it is lower than 1.12 (required for migration)
	```
	sudo cp migration/crun [where your crun is]
	```

> [!NOTE]
> You can use this command to check your crun version:
> ```
> crun --version

- Make the scripts executable
	```
	chmod +x setup/*.sh
	chmod +x launch/*.sh
	chmod +x monitor/*.sh
	```

	For scripts inside the `apps/` folder
	```
	find apps -name '*.sh' -exec chmod 775 {} \;
	```

- Install packages and dependencies
	```
	./setup/install_dependencies.sh
	```

- Install python packages using pip and `setup/requirements.txt`
	```
	python3 -m pip install -r setup/requirements.txt
	```

- Build executables from the C and C++ modules
	```
	./setup/build.sh
	```

- Set CPU affinity for Real-Time apps
	```
	./setup/set_cpu_affinity.sh [number_of_cpu] [target_folder]
	```

### On the host node:

- Build any C/ C++ modules in the `apps/` folder

- [As root] Launch the application(s) in another terminal

### On all nodes:

- Create a **new** file `data/nodes.txt` 
	- Fill it with the **public** IP addresses of remote nodes
	- File format mentioned [here](https://github.com/synercys/RT-Container-framework/tree/main/data#file-formats)

- Start the framework
	```
	python3 run.py
	```

<!---
- Create folders and files needed for running the framework
	```
	./setup/create_folders_and_files.sh
	```

- Assign ports for throughput measurement
	```
	python3 setup/assign_ports.py
	```

- Start the iperf3 and migration servers
	```
	./launch/run_servers.sh
	```

- Estimate the amount of resources needed to run the containers on all nodes in the network
	- Instructions [here](https://github.com/synercys/RT-Container-framework/tree/main/experiments/container_requirements#instructions)

- Set the `user` parameter in `launch/config.yaml`
	- This will be used to connect to the remote nodes

- Set and send the requirements from current node to remote nodes
	```
	python3 setup/set_container_requirement.py
	```

- Launch the framework in another terminal
	```
	./launch/run.sh
	```
- The components will run (in the background) for the duration specified by the `timeout` parameter in `launch/config.yaml`

- After timeout, stop the iperf3 and migration servers
	- Press <kbd>Ctrl</kbd> + C in the terminal where the servers were initially launched using `run_servers.sh`

- To run the experiments again,
	- Archive the contents and clean the `data/` folder
		```
		./setup/archive_data.sh
		```

	- Create folders and files needed
		```
		./setup/create_folders_and_files.sh
		```

- To delete executables and object files from the C and C++ modules, run
	```
	./setup/clean.sh
	```

### On the host node:

- Delete any executables and object files in the `apps/` folder
-->
