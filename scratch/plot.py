import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.ticker import MaxNLocator

# # tcp_llm_results
# cwnd = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_one_sender_results/cwnd.dat", delim_whitespace=True, header=None, names=["Time", "CWND"])
# queue_size = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_one_sender_results/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
# rtt = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_one_sender_results/rtt.dat", delim_whitespace=True, header=None, names=["Time", "RTT"])
# throughput = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_one_sender_results/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])

# #tcp_newreno_results
# cwnd = pd.read_csv("tcp_newreno_results/one_to_one/cwnd.dat", delim_whitespace=True, header=None, names=["Time", "CWND"])
# queue_size = pd.read_csv("tcp_newreno_results/one_to_one/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
# rtt = pd.read_csv("tcp_newreno_results/one_to_one/rtt.dat", delim_whitespace=True, header=None, names=["Time", "RTT"])
# throughput = pd.read_csv("tcp_newreno_results/one_to_one/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])


# #tcp_newreno_results_1h 1h_tcp_newreno_llm_results/tcp_newreno_results/one_to_one
# cwnd = pd.read_csv("1h_tcp_newreno_llm_results/tcp_newreno_results/one_to_one/cwnd.dat", delim_whitespace=True, header=None, names=["Time", "CWND"])
# queue_size = pd.read_csv("1h_tcp_newreno_llm_results/tcp_newreno_results/one_to_one/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
# rtt = pd.read_csv("1h_tcp_newreno_llm_results/tcp_newreno_results/one_to_one/rtt.dat", delim_whitespace=True, header=None, names=["Time", "RTT"])
# throughput = pd.read_csv("1h_tcp_newreno_llm_results/tcp_newreno_results/one_to_one/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])

#tcp_llm_results_1h 1h_tcp_newreno_llm_results/tcp_llm_results/one_to_one
cwnd = pd.read_csv("1h_tcp_newreno_llm_results/tcp_llm_results/one_to_one/cwnd.dat", delim_whitespace=True, header=None, names=["Time", "CWND"])
queue_size = pd.read_csv("1h_tcp_newreno_llm_results/tcp_llm_results/one_to_one/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
rtt = pd.read_csv("1h_tcp_newreno_llm_results/tcp_llm_results/one_to_one/rtt.dat", delim_whitespace=True, header=None, names=["Time", "RTT"])
throughput = pd.read_csv("1h_tcp_newreno_llm_results/tcp_llm_results/one_to_one/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])



# Calculate average values
cwnd_avg = cwnd["CWND"].mean()
queue_size_avg = queue_size["QueueSize"].mean()
def convert_ns_to_float(value):
    return float(value.replace("+", "").replace("ns", ""))
rtt_float = rtt["RTT"].apply(convert_ns_to_float)
rtt_avg = rtt_float.mean()
throughput = throughput.loc[throughput["Throughput"] <= 11]
throughput_avg = throughput["Throughput"].mean()

# Print the results
print(f"Average CWND: {cwnd_avg}")
print(f"Average Queue Size: {queue_size_avg}")
print(f"Average RTT: {rtt_avg}")
print(f"Average Throughput: {throughput_avg}")

# Line plots for each metric
plt.figure(figsize=(10, 6))
plt.plot(cwnd["Time"], cwnd["CWND"], label="CWND")
plt.title("CWND over Time")
plt.xlabel("Time")
plt.ylabel("CWND")
plt.legend()
plt.ylim(bottom=0)
# plt.savefig('plot/one/llm/cwnd.png')
# plt.savefig('plot/one/newreno/cwnd.png')
plt.savefig('1h_plot/one/llm/cwnd.png')
# plt.savefig('1h_plot/one/newreno/cwnd.png')
plt.show()

plt.figure(figsize=(10, 6))
plt.plot(queue_size["Time"], queue_size["QueueSize"], label="Queue Size", color='orange')
plt.title("Queue Size over Time")
plt.xlabel("Time")
plt.ylabel("Queue Size")
plt.legend()
plt.ylim(bottom=0)
# plt.savefig('plot/one/llm/queue.png')
# plt.savefig('plot/one/newreno/queue.png')
plt.savefig('1h_plot/one/llm/queue.png')
# plt.savefig('1h_plot/one/newreno/queue.png')
plt.show()

plt.figure(figsize=(10, 6))
plt.plot(rtt["Time"], pd.to_numeric(rtt["RTT"].str.replace("ns", ""), errors="coerce"), label="RTT", color='green')
plt.title("RTT over Time")
plt.xlabel("Time")
plt.ylabel("RTT (ms)")
plt.legend()
plt.ylim(bottom=0)
# plt.savefig('plot/one/llm/rtt.png')
# plt.savefig('plot/one/newreno/rtt.png')
plt.savefig('1h_plot/one/llm/rtt.png')
# plt.savefig('1h_plot/one/newreno/rtt.png')
plt.show()

plt.figure(figsize=(10, 6))
# throughput["Time"] = throughput["Time"].str.replace("ns", "").astype(float) / 1e9  # ns -> s
plt.plot(throughput["Time"], throughput["Throughput"], label="Throughput", color='red')
plt.title("Throughput over Time")
plt.xlabel("Time")
plt.ylabel("Throughput")
plt.ylim(bottom=0)
plt.gca().xaxis.set_major_locator(MaxNLocator(integer=True, prune='both'))  # Show integer labels only, prune unused edges
plt.legend()
plt.ylim(bottom=0)
# plt.savefig('plot/one/llm/throughput.png')
# plt.savefig('plot/one/newreno/throughput.png')
plt.savefig('1h_plot/one/llm/throughput.png')
# plt.savefig('1h_plot/one/newreno/throughput.png')
plt.show()
