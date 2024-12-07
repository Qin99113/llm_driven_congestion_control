import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.ticker import MaxNLocator


# Load method data
cwnd_method = pd.read_csv("newreno/cwnd.dat", delim_whitespace=True, header=None, names=["Time", "CWND"])
queue_size_method = pd.read_csv("newreno/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
rtt_method = pd.read_csv("newreno/rtt.dat", delim_whitespace=True, header=None, names=["Time", "RTT"])
throughput_method = pd.read_csv("newreno/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])

# Load baseline data
cwnd_baseline = pd.read_csv("tcp_llm_one_sender_results/cwnd.dat", delim_whitespace=True, header=None, names=["Time", "CWND"])
queue_size_baseline = pd.read_csv("tcp_llm_one_sender_results/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
rtt_baseline = pd.read_csv("tcp_llm_one_sender_results/rtt.dat", delim_whitespace=True, header=None, names=["Time", "RTT"])
throughput_baseline = pd.read_csv("tcp_llm_one_sender_results/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])


# Helper function to plot metrics
def plot_metric(metric_name, method_data, baseline_data, ylabel, output_file):
    plt.figure(figsize=(10, 6))
    plt.plot(method_data["Time"], method_data[metric_name], label="LLM", linestyle='-', marker='o')
    plt.plot(baseline_data["Time"], baseline_data[metric_name], label="newreno", linestyle='--', marker='x')
    plt.title(f"{metric_name} Comparison Over Time")
    plt.xlabel("Time")
    plt.ylabel(ylabel)
    plt.legend()
    plt.ylim(bottom=0)
    plt.gca().xaxis.set_major_locator(MaxNLocator(integer=True, prune='both'))  # Show integer labels only, prune unused edges
    plt.savefig(output_file)
    plt.show()

# Plot CWND
plot_metric("CWND", cwnd_method, cwnd_baseline, "CWND", "plot/cwnd_comparison.png")

# Plot Queue Size
plot_metric("QueueSize", queue_size_method, queue_size_baseline, "Queue Size", "plot/queue_comparison.png")

# Plot RTT (convert RTT to numeric if necessary)
rtt_method["RTT"] = rtt_method["RTT"].str.replace("ns", "").astype(float)
rtt_baseline["RTT"] = rtt_baseline["RTT"].str.replace("ns", "").astype(float)
plot_metric("RTT", rtt_method, rtt_baseline, "RTT (ns)", "plot/rtt_comparison.png")

# Plot Throughput
plot_metric("Throughput", throughput_method, throughput_baseline, "Throughput", "plot/throughput_comparison.png")