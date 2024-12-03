To run LLM example:

Move tcp-llm.h and tcp-llm.cc into <your NS3 path>/ns-allinone-3.37/ns-3.37/src/internet/model/

Move 0_1_prompts_test.py into <your NS3 path>/ns-allinone-3.37/ns-3.37/scratch/

Replace <your NS3 path>/ns-allinone-3.37/ns-3.37/src/internet/CMakeLists.txt with CMakeLists.txt

                  cd <your NS3 path>/ns-allinone-3.37/ns-3.37/build

                  cmake ..

                  make
                  
                  cd ..
                  
                  ./ns3 run scratch/tcp_llm_example.cc


4 output files will be stored at <your NS3 path>/ns-allinone-3.37/ns-3.37/llm-results_unique.

cwnd.dat

queueSize.dat

rtt.dat

throughput.dat
