### When to run?

To be done **before** launching the framework using `launch/run.sh`

### Experiment

- Run the container monitor and record the CPU and memory usages for 10 minutes
- Use it to estimate the resource requirements

### Instructions

Do the following on all nodes in the network. 

- Go to the `experiments/container_requirements` folder

- Make the script executable
    ```
    chmod +x run.sh
    ```

- Run the script and wait for 10 minutes
    ```
    ./run.sh
    ```

- For each container, 
    - the resource requirements on the current node are listed in `data/containers/[container id]/requirements.txt`

    - copy the requirements from each remote node and append to the aforementioned file (file format mentioned [here](https://github.com/synercys/RT-Container-framework/tree/main/data#file-formats))
