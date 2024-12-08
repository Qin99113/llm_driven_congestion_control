import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.ticker import MaxNLocator


# cwnd_baseline = pd.read_csv("tcp_newreno_results/one_to_one/cwnd.dat", delim_whitespace=True, header=None, names=["Time", "CWND"])
# queue_size_baseline = pd.read_csv("tcp_newreno_results/one_to_one/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
# rtt_baseline = pd.read_csv("tcp_newreno_results/one_to_one/rtt.dat", delim_whitespace=True, header=None, names=["Time", "RTT"])
# throughput_baseline = pd.read_csv("tcp_newreno_results/one_to_one/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])

# cwnd_method = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_one_sender_results/cwnd.dat", delim_whitespace=True, header=None, names=["Time", "CWND"])
# queue_size_method = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_one_sender_results/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
# rtt_method = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_one_sender_results/rtt.dat", delim_whitespace=True, header=None, names=["Time", "RTT"])
# throughput_method = pd.read_csv("tcp_llm_results/2min_best_tcp_llm_one_sender_results/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])

cwnd_baseline = pd.read_csv("1h_tcp_newreno_llm_results/tcp_newreno_results/one_to_one/cwnd.dat", delim_whitespace=True, header=None, names=["Time", "CWND"])
queue_size_baseline = pd.read_csv("1h_tcp_newreno_llm_results/tcp_newreno_results/one_to_one/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
rtt_baseline = pd.read_csv("1h_tcp_newreno_llm_results/tcp_newreno_results/one_to_one/rtt.dat", delim_whitespace=True, header=None, names=["Time", "RTT"])
throughput_baseline = pd.read_csv("1h_tcp_newreno_llm_results/tcp_newreno_results/one_to_one/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])


cwnd_method = pd.read_csv("1h_tcp_newreno_llm_results/tcp_llm_results/one_to_one/cwnd.dat", delim_whitespace=True, header=None, names=["Time", "CWND"])
queue_size_method = pd.read_csv("1h_tcp_newreno_llm_results/tcp_llm_results/one_to_one/queueSize.dat", delim_whitespace=True, header=None, names=["Time", "QueueSize"])
rtt_method  = pd.read_csv("1h_tcp_newreno_llm_results/tcp_llm_results/one_to_one/rtt.dat", delim_whitespace=True, header=None, names=["Time", "RTT"])
throughput_method = pd.read_csv("1h_tcp_newreno_llm_results/tcp_llm_results/one_to_one/throughput.dat", delim_whitespace=True, header=None, names=["Time", "Throughput"])



# Helper function to plot metrics
def plot_metric(metric_name, method_data, baseline_data, ylabel, output_file):
    plt.figure(figsize=(10, 6))
    plt.plot(method_data["Time"], method_data[metric_name], label="LLM", linestyle='-', marker='o')
    plt.plot(baseline_data["Time"], baseline_data[metric_name], label="newreno", linestyle='--', marker='x')
    plt.title(f"{metric_name} Comparison Over Time")
    plt.xlabel("Time")
    plt.ylabel(ylabel)
    plt.legend(loc='upper right')
    plt.ylim(bottom=0)
    plt.xlim(0, 3600) 
    plt.gca().xaxis.set_major_locator(MaxNLocator(integer=True, prune='both'))  # Show integer labels only, prune unused edges
    plt.savefig(output_file)
    plt.show()

# Plot CWND
# plot_metric("CWND", cwnd_method, cwnd_baseline, "CWND", "plot_comparison/one_to_one/cwnd_comparison.png")
plot_metric("CWND", cwnd_method, cwnd_baseline, "CWND", "1h_plot_comparison/one_to_one/cwnd_comparison.png")
# Plot Queue Size
# plot_metric("QueueSize", queue_size_method, queue_size_baseline, "Queue Size", "plot_comparison/one_to_one/queue_comparison.png")
plot_metric("QueueSize", queue_size_method, queue_size_baseline, "Queue Size", "1h_plot_comparison/one_to_one/queue_comparison.png")

# Plot RTT (convert RTT to numeric if necessary)
rtt_method["RTT"] = rtt_method["RTT"].str.replace("ns", "").astype(float)
rtt_baseline["RTT"] = rtt_baseline["RTT"].str.replace("ns", "").astype(float)
# plot_metric("RTT", rtt_method, rtt_baseline, "RTT (ms)", "plot_comparison/one_to_one/rtt_comparison.png")
plot_metric("RTT", rtt_method, rtt_baseline, "RTT (ms)", "1h_plot_comparison/one_to_one/rtt_comparison.png")

# # Plot Throughput
# # Note: Throughput Results in our generated data are in ns, so we need to convert to s here, but for new generate data, already in s, so no need to convert to s)
# throughput_baseline["Time"] = throughput_baseline["Time"].str.replace("ns", "").astype(float) / 1e9  # ns -> s
# throughput_method["Time"] = throughput_method["Time"].str.replace("ns", "").astype(float) / 1e9  # ns -> s

# plot_metric("Throughput", throughput_method, throughput_baseline, "Throughput", "plot_comparison/one_to_one/throughput_comparison.png")
throughput_method = throughput_method.loc[throughput_method["Throughput"] <= 11]
throughput_baseline = throughput_baseline.loc[throughput_baseline["Throughput"] <= 11]
plot_metric("Throughput", throughput_method, throughput_baseline, "Throughput", "1h_plot_comparison/one_to_one/throughput_comparison.png")