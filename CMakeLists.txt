# Minimum version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(test_tcp_project)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add ns-3 include paths (adjust the path based on your installation)
include_directories(/usr/include/ns3.35 ./inc)

# Link directories for ns-3 libraries (adjust the path based on your installation)
link_directories(/usr/lib/x86_64-linux-gnu)

# Define the executable
add_executable(tcp_newreno src/tcp_newreno_example.cc)

add_executable(tcp_llm src/tcp_llm_example.cc)

# Link the necessary ns-3 libraries
target_link_libraries(tcp_newreno
        ns3.35-core
        ns3.35-network
        ns3.35-internet
        ns3.35-applications
        ns3.35-traffic-control
        ns3.35-flow-monitor
        ns3.35-point-to-point
        ns3.35-csma
)

target_link_libraries(tcp_llm
        ns3.35-core
        ns3.35-network
        ns3.35-internet
        ns3.35-applications
        ns3.35-traffic-control
        ns3.35-flow-monitor
        ns3.35-point-to-point
        ns3.35-csma
)
