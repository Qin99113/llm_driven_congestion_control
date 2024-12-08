
//                                                 
// [sender0]---1000Mbps---  [Router1]---15Mbps---[Router2]     ---1000Mbps---[receiver0]
// [sender1]---1000Mbps---                                     ---1000Mbps---[receiver1]
// [sender2]---1000Mbps---                                     ---1000Mbps---[receiver2]

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"

using namespace ns3;

std::string dir;
std::map<uint32_t, uint64_t> prevFlowTxBytes;
Time prevTime = Seconds(0);

// Trace Throughput for all flows
void TraceThroughput(Ptr<FlowMonitor> monitor)
{
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    Time curTime = Now();
    // std::ofstream thr(dir + "/throughput.dat", std::ios::out | std::ios::app);

    // for (const auto& stat : stats)
    // {
    //     uint32_t flowId = stat.first;
    //     // Only Trace Flow with id 1,2,3
    //     if (0 < flowId && flowId < 4){
    //     const FlowMonitor::FlowStats& flowStats = stat.second;
    //     thr << "Flow " << flowId << " at time " << curTime.GetSeconds() << " s: "
    //         << 8 * (flowStats.txBytes - prevFlowTxBytes[flowId]) /
    //                 (1000 * 1000 * (curTime.GetSeconds() - prevTime.GetSeconds()))
    //         << " Mbps" << std::endl;
    //     prevFlowTxBytes[flowId] = flowStats.txBytes;
    //     }
    // }
    // prevTime = curTime;
    // Simulator::Schedule(Seconds(0.2), &TraceThroughput, monitor);
    std::ofstream thr(dir + "/detailed_throughput.dat", std::ios::out | std::ios::app);
    std::ofstream totalThr(dir + "/throughput.dat", std::ios::out | std::ios::app);
    double totalThroughput = 0.0; // accumulate throughput

    for (const auto& stat : stats)
    {
        uint32_t flowId = stat.first; // Flow ID
        // Only consider flows 1, 2, and 3
        if (0 < flowId && flowId < 4)
        {
            const FlowMonitor::FlowStats& flowStats = stat.second;
            double flowThroughput = 8.0 * (flowStats.txBytes - prevFlowTxBytes[flowId]) /
                        (1000.0 * 1000.0 * (curTime.GetSeconds() - prevTime.GetSeconds()));
            // thr << "Flow " << flowId << " at time " << curTime.GetSeconds() << " s: "
            //     << 8 * (flowStats.txBytes - prevFlowTxBytes[flowId]) /
            //             (1000 * 1000 * (curTime.GetSeconds() - prevTime.GetSeconds()))
            //     << " Mbps" << std::endl;
            thr << "Flow " << flowId << " at time " << curTime.GetSeconds() << " s: "
                << flowThroughput << " Mbps" << std::endl;
            totalThroughput += flowThroughput;
            // Update previous values for this flow
            prevFlowTxBytes[flowId] = flowStats.txBytes;
        }

    }
    // 写入总 throughput
    totalThr << curTime.GetSeconds() << " " << totalThroughput << std::endl;
    prevTime = curTime;
    Simulator::Schedule(Seconds(0.2), &TraceThroughput, monitor);
}

// Check the queue size
void
CheckQueueSize(Ptr<QueueDisc> qd)
{
    uint32_t qsize = qd->GetCurrentSize().GetValue();
    Simulator::Schedule(Seconds(0.2), &CheckQueueSize, qd);
    std::ofstream q(dir + "/queueSize.dat", std::ios::out | std::ios::app);
    q << Simulator::Now().GetSeconds() << " " << qsize << std::endl;
    q.close();
}

// Trace congestion window
static void
CwndTracer(Ptr<OutputStreamWrapper> stream, uint32_t oldval, uint32_t newval)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() << " " << newval / 1448.0 << std::endl;
}

void
TraceCwnd(uint32_t nodeId, uint32_t socketId)
{
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream(dir + "cwnd_" + std::to_string(nodeId) + ".dat");
    Config::ConnectWithoutContext("/NodeList/" + std::to_string(nodeId) +
                                      "/$ns3::TcpL4Protocol/SocketList/" +
                                      std::to_string(socketId) + "/CongestionWindow",
                                  MakeBoundCallback(&CwndTracer, stream));
}

// Trace RTT
void RttTracer(Ptr<OutputStreamWrapper> stream, ns3::Time oldval, ns3::Time newval)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() << " " << newval / 1000000 << std::endl;
}

void TraceRtt(uint32_t nodeId, uint32_t socketId)
{
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream(dir + "rtt_" + std::to_string(nodeId) + ".dat");
    Config::ConnectWithoutContext("/NodeList/" + std::to_string(nodeId) +
                                  "/$ns3::TcpL4Protocol/SocketList/" +
                                  std::to_string(socketId) + "/RTT",
                                  MakeBoundCallback(&RttTracer, stream));
}


// Main Function
int main(int argc, char* argv[])
{
    uint32_t numSenders = 3;
    uint32_t numReceivers = 3;
    // Naming the output directory using local system time
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%d-%m-%Y-%I-%M-%S", timeinfo);
    std::string currentTime(buffer);
    dir = "tcp_llm_multi_sender_receiver_results/";

    std::string tcpTypeId = "TcpNewReno";
    std::string queueDisc = "FifoQueueDisc";
    uint32_t delAckCount = 2;
    bool bql = true;
    bool enablePcap = false;
    Time stopTime = Seconds(120);

    CommandLine cmd(__FILE__);
    cmd.AddValue("tcpTypeId", "Transport protocol to use: TcpNewReno, TcpBbr", tcpTypeId);
    cmd.AddValue("delAckCount", "Delayed ACK count", delAckCount);
    cmd.AddValue("enablePcap", "Enable/Disable pcap file generation", enablePcap);
    cmd.AddValue("stopTime",
                 "Stop time for applications / simulation time will be stopTime + 1",
                 stopTime);
    cmd.Parse(argc, argv);

    queueDisc = std::string("ns3::") + queueDisc;

    std::string tcpVariant = "TcpLlm"; 
    // std::string tcpVariant = "TcpNewReno"; 
    tcpVariant = std::string ("ns3::") + tcpVariant;
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
    Config::SetDefault("ns3::TcpLlm::ThroughputFilePath", StringValue("./"+dir+"throughput.dat"));
    Config::SetDefault("ns3::TcpLlm::TopolgyDescription", StringValue("Current network topology with three senders S0 S1 S2, two routers R1 R2, and three receivers RE0 RE1 RE2, where: Links from all senders to R1 have a bandwidth of 1000 Mbps with a delay of 5 ms respectivaly; Link between R1 and R2 (the bottleneck) has a bandwidth of 15 Mbps and a delay of 10 ms; Link from R2 to all three receivers have a bandwidth of 1000 Mbps with a delay of 5 ms respectively."));
    // Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::" + tcpTypeId));
    Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(4194304));
    Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(6291456));
    Config::SetDefault("ns3::TcpSocket::InitialCwnd", UintegerValue(10));
    Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(delAckCount));
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1448));
    Config::SetDefault("ns3::DropTailQueue<Packet>::MaxSize", QueueSizeValue(QueueSize("1p")));
    Config::SetDefault(queueDisc + "::MaxSize", QueueSizeValue(QueueSize("100p")));


    // Create nodes
    NodeContainer senders;
    NodeContainer receivers;
    NodeContainer routers;
    senders.Create(numSenders);
    receivers.Create(numReceivers);
    routers.Create(2);

    // Create links
    PointToPointHelper bottleneckLink;
    bottleneckLink.SetDeviceAttribute("DataRate", StringValue("15Mbps"));
    bottleneckLink.SetChannelAttribute("Delay", StringValue("10ms"));

    PointToPointHelper edgeLink1000;
    edgeLink1000.SetDeviceAttribute("DataRate", StringValue("1000Mbps"));
    edgeLink1000.SetChannelAttribute("Delay", StringValue("5ms"));

    PointToPointHelper edgeLink500;
    edgeLink500.SetDeviceAttribute("DataRate", StringValue("1000Mbps"));
    edgeLink500.SetChannelAttribute("Delay", StringValue("5ms"));

    PointToPointHelper edgeLink800;
    edgeLink800.SetDeviceAttribute("DataRate", StringValue("1000Mbps"));
    edgeLink800.SetChannelAttribute("Delay", StringValue("5ms"));

    // Install links
    std::vector<NetDeviceContainer> senderEdges(numSenders);
    std::vector<NetDeviceContainer> receiverEdges(numReceivers);
    // NetDeviceContainer senderEdges[numSenders];
    for (uint32_t i = 0; i < numSenders; ++i)
    {
        senderEdges[i] = edgeLink1000.Install(senders.Get(i), routers.Get(0));
    }

    NetDeviceContainer bottleneckDevices = bottleneckLink.Install(routers.Get(0), routers.Get(1));

    // NetDeviceContainer receiverEdges[numReceivers];
    receiverEdges[0] = edgeLink500.Install(routers.Get(1), receivers.Get(0));
    receiverEdges[1] = edgeLink800.Install(routers.Get(1), receivers.Get(1));
    receiverEdges[2] = edgeLink1000.Install(routers.Get(1), receivers.Get(2));

    // Install Internet stack
    InternetStackHelper internet;
    internet.Install(senders);
    internet.Install(receivers);
    internet.Install(routers);


    // **Add TrafficControlHelper here**
    TrafficControlHelper tch;
    tch.SetRootQueueDisc("ns3::FifoQueueDisc"); // Example: Use FifoQueueDisc

    if (bql)
    {
        tch.SetQueueLimits("ns3::DynamicQueueLimits", "HoldTime", StringValue("1000ms"));
    }

    // Install queue disciplines for each link
    for (uint32_t i = 0; i < numSenders; ++i)
    {
        tch.Install(senderEdges[i]); // For sender to router links
    }
    tch.Install(bottleneckDevices); // For bottleneck link
    for (uint32_t i = 0; i < numReceivers; ++i)
    {
        tch.Install(receiverEdges[i]); // For router to receiver links
    }

    // Assign IP addresses
    Ipv4AddressHelper ipv4;
    // Ipv4InterfaceContainer senderInterfaces[numSenders];
    // Ipv4InterfaceContainer receiverInterfaces[numReceivers];

    std::vector<Ipv4InterfaceContainer> senderInterfaces(numSenders);
    std::vector<Ipv4InterfaceContainer> receiverInterfaces(numReceivers);

    for (uint32_t i = 0; i < numSenders; ++i)
    {
        ipv4.SetBase(("10.1." + std::to_string(i + 1) + ".0").c_str(), "255.255.255.0");
        senderInterfaces[i] = ipv4.Assign(senderEdges[i]);
    }

    ipv4.SetBase("10.2.0.0", "255.255.255.0");
    Ipv4InterfaceContainer bottleneckInterfaces = ipv4.Assign(bottleneckDevices);

    for (uint32_t i = 0; i < numReceivers; ++i)
    {
        ipv4.SetBase(("10.3." + std::to_string(i + 1) + ".0").c_str(), "255.255.255.0");
        receiverInterfaces[i] = ipv4.Assign(receiverEdges[i]);
    }

    // Populate routing tables
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();


    uint16_t port = 50001;
    for (uint32_t i = 0; i < 3; ++i)
    {
        // BulkSendHelper
        BulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(receiverInterfaces[i].GetAddress(1), port));
        source.SetAttribute("MaxBytes", UintegerValue(0));
        ApplicationContainer sourceApps = source.Install(senders.Get(i));
        sourceApps.Start(Seconds(0.1));
        // Hook trace source after application starts // tracing sender 0
        Simulator::Schedule(Seconds(0.1) + MilliSeconds(1), &TraceCwnd, i, 0);
        sourceApps.Stop(stopTime);
        // PacketSinkHelper
        PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        ApplicationContainer sinkApps = sink.Install(receivers.Get(i));
        sinkApps.Start(Seconds(0.0));
        sinkApps.Stop(stopTime);

        port++; // 下一个流使用新的端口号
    }
    // Create a new directory to store the output of the program
    
    std::string dirToSave = "mkdir -p " + dir;
    if (system(dirToSave.c_str()) == -1)
    {
        exit(1);
    }

    std::cout << "Topology complete" << std::endl;

    // Trace the queue occupancy on the second interface of R1
    tch.Uninstall(routers.Get(0)->GetDevice(3));
    QueueDiscContainer qd;
    qd = tch.Install(routers.Get(0)->GetDevice(3));
    // Check if queueSize.dat exist, if so, truncate it
    std::ofstream thr1(dir + "/queueSize.dat", std::ios::out | std::ios::trunc);
    thr1.close();
    Simulator::ScheduleNow(&CheckQueueSize, qd.Get(0));

    std::cout << "Enable PCAP traces" << std::endl;

    // Generate PCAP traces if it is enabled
    if (enablePcap)
    {
        if (system((dirToSave + "/pcap/").c_str()) == -1)
        {
            exit(1);
        }
        bottleneckLink.EnablePcapAll(dir + "/pcap/newreno", true);
    }

    // Create Flow Monitor
    FlowMonitorHelper flowmonHelper;
    Ptr<FlowMonitor> monitor = flowmonHelper.InstallAll();

    // Check if throughput.dat exist, if so, truncate it
    std::ofstream thr2(dir + "/throughput.dat", std::ios::out | std::ios::trunc);
    thr2.close();
    std::ofstream thr3(dir + "/detailed_throughput.dat", std::ios::out | std::ios::trunc);
    thr3.close();

    // Trace Throughput
    Simulator::Schedule(Seconds(0 + 0.000001), &TraceThroughput, monitor);




    std::cout << "Schedule RTT tracing" << std::endl;

    // Schedule RTT tracking
    for (uint32_t i = 0; i < numSenders; ++i)
    {
         Simulator::Schedule(Seconds(0.2) + MilliSeconds(1), &TraceRtt, i, 0); // Trace RTT for each flow
    }

    // Run simulation
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();

    Simulator::Destroy();
    return 0;
}
