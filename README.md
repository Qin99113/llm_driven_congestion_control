# README

# LLM-Driven Congestion Control Algorithm

## Baseline Congestion Control Algorithm
- **TCP NewReno**

## LLM Used
- **OpenAI** gpt-4o mini and gpt-o1 preview

---

## Implementation

### Environment Setup
- **NS-3 Version:** Tested on NS-3.37 (other versions of NS-3 may also work).

### Steps to Set Up
1. Move the following files into their respective directories:
   - **Congestion Control Algorithm:**
     - `tcp-llm.h` → `<your NS3 path>/ns-allinone-3.37/ns-3.37/src/internet/model/`
     - `tcp-llm.cc` → `<your NS3 path>/ns-allinone-3.37/ns-3.37/src/internet/model/`
   - **Experiment Scripts:**
     - `0_1_prompts_test.py` → `<your NS3 path>/ns-allinone-3.37/ns-3.37/scratch/`
     - `tcp_llm_example.cc` → `<your NS3 path>/ns-allinone-3.37/ns-3.37/scratch/`
     - `tcp_llm_multi_sender.cc` → `<your NS3 path>/ns-allinone-3.37/ns-3.37/scratch/`
     - `tcp_llm_multi_compete.cc` → `<your NS3 path>/ns-allinone-3.37/ns-3.37/scratch/`
     - `tcp_llm_multi_se_rec.cc` → `<your NS3 path>/ns-allinone-3.37/ns-3.37/scratch/`
   - **Plotting Scripts and Folders:**
     - Move `tcp_llm_results` folder to `<your NS3 path>/ns-allinone-3.37/ns-3.37/`.
     - Move `tcp_newreno_results` folder to `<your NS3 path>/ns-allinone-3.37/ns-3.37/`.
     - Move `/plot` folder, `/plot_comparision` folder to `<your NS3 path>/ns-allinone-3.37/ns-3.37/`.
     - Move `plot_<>.py` and `plot_both_<>.py` to `<your NS3 path>/ns-allinone-3.37/ns-3.37/scratch/`.

2. Update the CMakeLists.txt file:
   - Replace the existing `<your NS3 path>/ns-allinone-3.37/ns-3.37/src/internet/CMakeLists.txt` with the provided `CMakeLists.txt`.
   - **Or** manually add:
     - `model/tcp-llm.h` to `set(header_files...)`.
     - `model/tcp-llm.cc` to `set(source_files...)`.

3. Build NS-3:
   ```bash
   cd <your NS3 path>/ns-allinone-3.37/ns-3.37/build
   cmake ..
   make
   cd ..

4. Run the experiments:
   - `tcp_llm_example.cc`: This is a simple example to show how to use the LLM algorithm and results will be collected in the folder `tcp_llm_one_sender_results`.
   - `tcp_llm_multi_sender.cc`: This is an example to show how to use the LLM algorithm with multiple senders and results will be collected in the folder `tcp_llm_multi_sender_results`.
   - `tcp_llm_multi_compete.cc`: This is an example to show how to use the LLM algorithm with multiple senders and multiple receivers and results will be collected in the folder `tcp_llm_multi_compete_results`.
   - `tcp_llm_multi_se_rec.cc`: This is an example to show how to use the LLM algorithm with multiple senders and multiple receivers and results will be collected in the folder `tcp_llm_multi_se_rec_results`.
   - **Note:** You can change the `TcpVariant` parameter in the code to either `TcpNewReno` or `TcpLlm` to run the experiment for both variants.
   - **Note:** You can change the `stopTime` parameter in the code to run the experiment for different simulation times.
   ```bash
   ./ns3 run scratch/tcp_llm_example.cc
   ```
   - **Note:** You need to store the result folders in a separate directory for each experiment due to overwriting of results.

### Results
1. The results will be stored in the respective folders mentioned in the previous section.
2. Use the provided `plot_<>.py` and `plot_both_<>.py` scripts to plot the results.
3. The average values of the 4 metrics will be printed out.

## Output Description
Each output file contains data for specific metrics:
1. **`cwnd.dat`**:
   - Time-series data of the congestion window size for each TCP variant.
   - Units: Segments.

2. **`queueSize.dat`**:
   - Records the size of the queue at the bottleneck link.
   - Units: Packets.

3. **`rtt.dat`**:
   - Tracks the Round-Trip Time (RTT) for each TCP flow.
   - Units: Milliseconds (ms).

4. **`throughput.dat`**:
   - Logs throughput over time for each flow.
   - Units: Megabits per second (Mbps).

## Directory Structure
```
<your NS3 path>/ns-allinone-3.37/ns-3.37/
│
├── src/
│   ├── internet/
│   │   └── model/
│   │   |    ├── tcp-llm.h
│   │   |    └── tcp-llm.cc
|   |   ├──CMakeLists.txt
|   |   └──...
│   └── ...
│
├── scratch/
│   ├── 0_1_prompts_test.py
│   ├── tcp_llm_example.cc
│   ├── tcp_llm_multi_sender.cc
│   ├── tcp_llm_multi_compete.cc
│   ├── tcp_llm_multi_se_rec.cc
|   ├── plot_<>.py
│   └── plot_both_<>.py
|
│
├── tcp_newreno_results/
│   ├── one_to_one/
│   ├── tcp_llm_multi_sender_results/
|   ├── tcp_llm_multi_sender_receiver_results/
│   └── tcp_llm_multi_sender_compete/
|
│
├── tcp_llm_results/
|   ├── 2min_best_tcp_llm_one_sender_results/
│   ├── 2min_best_tcp_llm_multi_sender_results/
│   ├── 2min_best_tcp_llm_multi_sender_compete/
│   └──2min_best_tcp_llm_multi_sender_receiver_results/
│  
│
├── plot/
│ 
│  
│
└── plot_comparison/
```
## Troubleshooting

### Common Issues and Fixes

1. **Missing `tcp-llm` files:**
   - Ensure `tcp-llm.h` and `tcp-llm.cc` are correctly placed in `<your NS3 path>/src/internet/model/`.

2. **CMake Errors:**
   - Ensure `CMakeLists.txt` is updated to include `tcp-llm.h` and `tcp-llm.cc` in the `set(header_files...)` and `set(source_files...)` sections.

3. **Runtime Errors:**
   - Check the paths in the experiment scripts (`scratch/` folder) to ensure they match your local setup.

4. **Queue Size Stays Zero:**
   - Verify that the bottleneck queue size is set correctly using the `BDP` formula.
   - Example: `Config::SetDefault("ns3::DropTailQueue<Packet>::MaxSize", QueueSizeValue(QueueSize("BDP packets")));`
   - Verify that the bottleneck link is correctly connected to the queue.
   - Example: Number 1 means the router connects to the bottleneck link.
```
                tch.Uninstall(routers.Get(0)->GetDevice(1));
                QueueDiscContainer qd;
                qd = tch.Install(routers.Get(0)->GetDevice(1));

```