import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.ticker import MaxNLocator

# tcp_llm_results
cwnd0 = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_multi_sender_receiver_results/cwnd_0.dat", delim_whitespace=True, header=None, names=["Time", "CWND0"])
cwnd1 = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_multi_sender_receiver_results/cwnd_1.dat", delim_whitespace=True, header=None, names=["Time", "CWND1"])
cwnd2 = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_multi_sender_receiver_results/cwnd_2.dat", delim_whitespace=True, header=None, names=["Time", "CWND2"])
queue_size = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_multi_sender_receiver_results/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
rtt0 = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_multi_sender_receiver_results/rtt_0.dat", delim_whitespace=True, header=None, names=["Time", "RTT0"])
rtt1 = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_multi_sender_receiver_results/rtt_1.dat", delim_whitespace=True, header=None, names=["Time", "RTT1"])
rtt2 = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_multi_sender_receiver_results/rtt_2.dat", delim_whitespace=True, header=None, names=["Time", "RTT2"])
throughput = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_multi_sender_receiver_results/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])


# # tcp_newreno_results
# cwnd0 = pd.read_csv("tcp_newreno_results/tcp_llm_multi_sender_receiver_results/cwnd_0.dat", delim_whitespace=True, header=None, names=["Time", "CWND0"])
# cwnd1 = pd.read_csv("tcp_newreno_results/tcp_llm_multi_sender_receiver_results/cwnd_1.dat", delim_whitespace=True, header=None, names=["Time", "CWND1"])
# cwnd2 = pd.read_csv("tcp_newreno_results/tcp_llm_multi_sender_receiver_results/cwnd_2.dat", delim_whitespace=True, header=None, names=["Time", "CWND2"])
# queue_size = pd.read_csv("tcp_newreno_results/tcp_llm_multi_sender_receiver_results/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
# rtt0 = pd.read_csv("tcp_newreno_results/tcp_llm_multi_sender_receiver_results/rtt_0.dat", delim_whitespace=True, header=None, names=["Time", "RTT0"])
# rtt1 = pd.read_csv("tcp_newreno_results/tcp_llm_multi_sender_receiver_results/rtt_1.dat", delim_whitespace=True, header=None, names=["Time", "RTT1"])
# rtt2 = pd.read_csv("tcp_newreno_results/tcp_llm_multi_sender_receiver_results/rtt_2.dat", delim_whitespace=True, header=None, names=["Time", "RTT2"])
# throughput = pd.read_csv("tcp_newreno_results/tcp_llm_multi_sender_receiver_results/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])


# Calculate average values
cwnd_avg0 = cwnd0["CWND0"].mean()
cwnd_avg1 = cwnd1["CWND1"].mean()
cwnd_avg2 = cwnd2["CWND2"].mean()
queue_size_avg = queue_size["QueueSize"].mean()
def convert_ns_to_float(value):
    return float(value.replace("+", "").replace("ns", ""))
rtt_float0 = rtt0["RTT0"].apply(convert_ns_to_float)
rtt_avg0 = rtt_float0.mean()
rtt_float1 = rtt1["RTT1"].apply(convert_ns_to_float)
rtt_avg1 = rtt_float1.mean()
rtt_float2 = rtt2["RTT2"].apply(convert_ns_to_float)
rtt_avg2 = rtt_float2.mean()
throughput_avg = throughput["Throughput"].mean()

# Print the results
print(f"Average CWND0: {cwnd_avg0}")
print(f"Average CWND1: {cwnd_avg1}")
print(f"Average CWND2: {cwnd_avg2}")
print(f"Average Queue Size: {queue_size_avg}")
print(f"Average RTT0: {rtt_avg0}")
print(f"Average RTT1: {rtt_avg1}")
print(f"Average RTT2: {rtt_avg2}")
print(f"Average Throughput: {throughput_avg}")

# Line plots for each metric
plt.figure(figsize=(10, 6))
plt.plot(cwnd0["Time"], cwnd0["CWND0"], label="CWND0")
plt.plot(cwnd1["Time"], cwnd1["CWND1"], label="CWND1")
plt.plot(cwnd2["Time"], cwnd2["CWND2"], label="CWND2")
plt.title("CWND over Time")
plt.xlabel("Time")
plt.ylabel("CWND")
plt.legend()
plt.ylim(bottom=0)
plt.savefig('plot/fairness/llm/cwnd.png')
# plt.savefig('plot/fairness/newreno/cwnd.png')
plt.show()

plt.figure(figsize=(10, 6))
plt.plot(queue_size["Time"], queue_size["QueueSize"], label="Queue Size", color='orange')
plt.title("Queue Size over Time")
plt.xlabel("Time")
plt.ylabel("Queue Size")
plt.legend()
plt.ylim(bottom=0)
plt.savefig('plot/fairness/llm/queue.png')
# plt.savefig('plot/fairness/newreno/queue.png')
plt.show()

plt.figure(figsize=(10, 6))
plt.plot(rtt0["Time"], pd.to_numeric(rtt0["RTT0"].str.replace("ns", ""), errors="coerce"), label="RTT0", color='green')
plt.plot(rtt1["Time"], pd.to_numeric(rtt1["RTT1"].str.replace("ns", ""), errors="coerce"), label="RTT1", color='blue')
plt.plot(rtt2["Time"], pd.to_numeric(rtt2["RTT2"].str.replace("ns", ""), errors="coerce"), label="RTT2", color='orange')
plt.title("RTT over Time")
plt.xlabel("Time")
plt.ylabel("RTT (ms)")
plt.legend()
plt.ylim(bottom=0)
plt.savefig('plot/fairness/llm/rtt.png')
# plt.savefig('plot/fairness/newreno/rtt.png')
plt.show()

plt.figure(figsize=(10, 6))
plt.plot(throughput["Time"], throughput["Throughput"], label="Throughput", color='red')
plt.title("Throughput over Time")
plt.xlabel("Time")
plt.ylabel("Throughput")
plt.ylim(bottom=0)
plt.gca().xaxis.set_major_locator(MaxNLocator(integer=True, prune='both'))  # Show integer labels only, prune unused edges
plt.legend()
plt.ylim(bottom=0)
plt.savefig('plot/fairness/llm/throughput.png')
# plt.savefig('plot/fairness/newreno/throughput.png')
plt.show()
