## LLM Driven Congestion Control Algorithm 

Baseline Congestion Control Algorithm: TCP Newreno

LLM: OpenAI

## Implementation

Environment Set up version NS-3 37 and other versions of NS3 will also be available.

To run LLM example:

Move tcp-llm.h and tcp-llm.cc into \<your NS3 path\>/ns-allinone-3.37/ns-3.37/src/internet/model/

Move 0_1_prompts_test.py into \<your NS3 path\>/ns-allinone-3.37/ns-3.37/scratch/

Move tcp_llm_example.cc in to \<your NS3 path\>/ns-allinone-3.37/ns-3.37/scratch/

Move tcp_llm_multi_sender.cc in to \<your NS3 path\>/ns-allinone-3.37/ns-3.37/scratch/

Move tcp_llm_multi_compete.cc in to \<your NS3 path\>/ns-allinone-3.37/ns-3.37/scratch/

Move tcp_llm_multi_se_rec.cc in to \<your NS3 path\>/ns-allinone-3.37/ns-3.37/scratch/

Replace \<your NS3 path\>/ns-allinone-3.37/ns-3.37/src/internet/CMakeLists.txt with CMakeLists.txt or \<add model/tcp-llm.h to set(header_files.... and add model/tcp-llm.cc to set(source_files...in CMakeLists.txt>

                  cd \<your NS3 path\>/ns-allinone-3.37/ns-3.37/build

                  cmake ..

                  make
                  
                  cd ..
                  
                  ./ns3 run scratch/tcp_llm_example.cc

/plot folder, /newreno folder, plot_both.py, plot.py should be moved to /ns-3.37

### Experiment
YOU CAN RUN THE EXPERIMENTS BY CHANGING THE PATHS IN THE CODE AND RUNNING THE EXPERIMENTS IN THE FOLLOWING ORDER:

tcp_llm_example.cc: This is a simple example to show how to use the LLM algorithm and results will be collected in the folder tcp_llm_one_sender_results.
<Note you can change Tcpvariant to either TcpNewReno or TcpLlm and run the experiment for both variants>


### Results

1. tcp_newreno_results and tcp_llm_results folders are results collected from 2min simulations with one sender and one receiver, muti-senders and one receiver, multi-senders and multi-receivers.
2. We also conduct one additional experiment in one hour simulation to compare the performance of the two methods, you can find the results in the folder named begin wtih "1h_".

4 output files will be stored at \<your NS3 path\>/ns-allinone-3.37/ns-3.37/<results path defined>.

cwnd.dat

queueSize.dat

rtt.dat

throughput.dat

Use plot_<>.py to plot the 4 metrics for our llm-driven method or baseline newreno, the figures will be saved in the plot folder, the average values of the 4 metrics will be printed out;

Use plot_both_<>.py to plot the 4 metrics for both method together, the figures will be saved in the plot_comparision folder;

                  python  scratch/<plot files you are ready to run>


