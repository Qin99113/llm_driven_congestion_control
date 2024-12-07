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
the newreno's 60s results has been saved in newreno folder;
use plot.py to plot the 4 metrics for our method or baseline newreno, the figures will be saved in the plot folder, the average values of the 4 metrics will be printed out;
use plot_both.py to plot the 4 metrics for both method together, the figures will be saved in the plot folder;

the /one_one_result has the figures for the 1-1-result

4 output files will be stored at \<your NS3 path\>/ns-allinone-3.37/ns-3.37/<results path defined>.

cwnd.dat

queueSize.dat

rtt.dat

throughput.dat
