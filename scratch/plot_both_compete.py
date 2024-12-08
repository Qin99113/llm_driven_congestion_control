import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.ticker import MaxNLocator


cwnd_baseline = pd.read_csv("tcp_newreno_results/tcp_llm_multi_sender_compete/cwnd_1.dat", delim_whitespace=True, header=None, names=["Time", "CWND"])
queue_size_baseline = pd.read_csv("tcp_newreno_results/tcp_llm_multi_sender_compete/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
rtt_baseline = pd.read_csv("tcp_newreno_results/tcp_llm_multi_sender_compete/rtt_1.dat", delim_whitespace=True, header=None, names=["Time", "RTT"])
throughput_baseline = pd.read_csv("tcp_newreno_results/tcp_llm_multi_sender_compete/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])

cwnd_method = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_multi_sender_compete/cwnd_1.dat", delim_whitespace=True, header=None, names=["Time", "CWND"])
queue_size_method = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_multi_sender_compete/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
rtt_method = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_multi_sender_compete/rtt_1.dat", delim_whitespace=True, header=None, names=["Time", "RTT"])
throughput_method = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_multi_sender_compete/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])


# Helper function to plot metrics
def plot_metric(metric_name, method_data, baseline_data, ylabel, output_file):
    plt.figure(figsize=(10, 6))
    plt.plot(method_data["Time"], method_data[metric_name], label="LLM", linestyle='-', marker='o')
    plt.plot(baseline_data["Time"], baseline_data[metric_name], label="newreno", linestyle='--', marker='x')
    plt.title(f"{metric_name} Comparison Over Time")
    plt.xlabel("Time")
    plt.ylabel(ylabel)
    plt.legend(loc='upper right')
    plt.xlim(0, 60)
    # plt.ylim(0, 200)
    plt.gca().xaxis.set_major_locator(MaxNLocator(integer=True, prune='both'))  # Show integer labels only, prune unused edges
    plt.savefig(output_file)
    plt.show()

# Plot CWND
plot_metric("CWND", cwnd_method, cwnd_baseline, "CWND", "plot_comparison/compete/cwnd_comparison.png")

# Plot Queue Size
plot_metric("QueueSize", queue_size_method, queue_size_baseline, "Queue Size", "plot_comparison/compete/queue_comparison.png")

# Plot RTT (convert RTT to numeric if necessary)
rtt_method["RTT"] = rtt_method["RTT"].str.replace("ns", "").astype(float)
rtt_baseline["RTT"] = rtt_baseline["RTT"].str.replace("ns", "").astype(float)
plot_metric("RTT", rtt_method, rtt_baseline, "RTT (ms)", "plot_comparison/compete/rtt_comparison.png")

# Plot Throughput
throughput_method = throughput_method.loc[throughput_method["Throughput"] <= 16]
throughput_baseline = throughput_baseline.loc[throughput_baseline["Throughput"] <= 16]
plot_metric("Throughput", throughput_method, throughput_baseline, "Throughput", "plot_comparison/compete/throughput_comparison.png")