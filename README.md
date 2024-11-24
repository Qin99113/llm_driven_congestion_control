To run LLM example:

Move tcp-llm.h and tcp-llm.cc into <your NS3 path>/ns-allinone-3.37/ns-3.37/src/internet/model/
Replace <your NS3 path>/ns-allinone-3.37/ns-3.37/src/internet/CMakeLists.txt with CMakeLists.txt

cd into <your NS3 path>/ns-allinone-3.37/ns-3.37/build
type in terminal: cmake ..
                  make
                  cd ..
                  ./ns3 run scratch/tcp_llm_example.cc
