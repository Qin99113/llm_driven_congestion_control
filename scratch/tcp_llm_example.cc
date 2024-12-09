/*
 * Copyright (c) 2024 Xiaoxuan Qin and Yufei Wang
 * Licensed under the MIT License (see LICENSE file).
 * Based on the work of Aarti Nandagiri <aarti.nandagiri@gmail.com>, Vivek Jain <jain.vivek.anand@gmail.com>
 * and Mohit P. Tahiliani <tahiliani@nitk.edu.in> at https://www.nsnam.org/docs/release/3.34/doxygen/tcp-bbr-example_8cc_source.html.
 */

// This program simulates the following topology:
//
//           1000 Mbps           10Mbps          1000 Mbps
//  Sender -------------- R1 -------------- R2 -------------- Receiver
//              5ms               10ms               5ms
//
// The link between R1 and R2 is a bottleneck link with 10 Mbps. All other
// links are 1000 Mbps.
//


#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"

using namespace ns3;

std::string dir;
uint32_t prev = 0;
Time prevTime = Seconds(0);

// Calculate throughput
static void
TraceThroughput(Ptr<FlowMonitor> monitor)
{
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    auto itr = stats.begin();
    Time curTime = Now();
    std::ofstream thr(dir + "/throughput.dat", std::ios::out | std::ios::app);
    thr << curTime.GetSeconds() << " "
        << 8 * (itr->second.txBytes - prev) /
               (1000 * 1000 * (curTime.GetSeconds() - prevTime.GetSeconds()))
        << std::endl;
    prevTime = curTime;
    prev = itr->second.txBytes;
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
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream(dir + "/cwnd.dat");
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
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream(dir + "rtt.dat");
    Config::ConnectWithoutContext("/NodeList/" + std::to_string(nodeId) +
                                  "/$ns3::TcpL4Protocol/SocketList/" +
                                  std::to_string(socketId) + "/RTT",
                                  MakeBoundCallback(&RttTracer, stream));
}

int
main(int argc, char* argv[])
{
    // Naming the output directory using local system time
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%d-%m-%Y-%I-%M-%S", timeinfo);
    std::string currentTime(buffer);
    dir = "tcp_llm_one_sender_results/";

    std::string tcpTypeId = "TcpNewReno";
    std::string queueDisc = "FifoQueueDisc";
    uint32_t delAckCount = 2;
    bool bql = true;
    bool enablePcap = false;
    Time stopTime = Seconds(3600);

    CommandLine cmd(__FILE__);
    cmd.AddValue("tcpTypeId", "Transport protocol to use: TcpNewReno, TcpBbr", tcpTypeId);
    cmd.AddValue("delAckCount", "Delayed ACK count", delAckCount);
    cmd.AddValue("enablePcap", "Enable/Disable pcap file generation", enablePcap);
    cmd.AddValue("stopTime",
                 "Stop time for applications / simulation time will be stopTime + 1",
                 stopTime);
    cmd.Parse(argc, argv);

    queueDisc = std::string("ns3::") + queueDisc;

    // std::string tcpVariant = "TcpLlm"; 
    std::string tcpVariant = "TcpNewReno";
    tcpVariant = std::string ("ns3::") + tcpVariant;
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
    Config::SetDefault("ns3::TcpLlm::ThroughputFilePath", StringValue("./"+dir+"throughput.dat"));
    Config::SetDefault("ns3::TcpLlm::TopolgyDescription", StringValue("Current network topology with a sender, R1, R2, and a receiver, where: Link from the sender to R1 has a bandwidth of 1000 Mbps with a delay of 5 ms; Link between R1 and R2 (the bottleneck) has a bandwidth of 10 Mbps and a delay of 10 ms; Link from R2 to the receiver has a bandwidth of 1000 Mbps with a delay of 5 ms."));
    // Config::SetDefault("ns3::TcpLlm::TopolgyDescription", StringValue("Current network topology with a sender, R1, R2, and a receiver, where: \n Sender to R1: Bandwidth 1000 Mbps, Delay 5ms \n R1 to R2(bottleneck link): Bandwidth 100 Mbps, Delay 10ms \n R2 to receiver: Bandwidth 1000 Mbps, Delay 5ms"));
    // Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::" + tcpTypeId));
    Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(4194304));
    Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(6291456));
    Config::SetDefault("ns3::TcpSocket::InitialCwnd", UintegerValue(10));
    Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(delAckCount));
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1448));
    Config::SetDefault("ns3::DropTailQueue<Packet>::MaxSize", QueueSizeValue(QueueSize("1p")));
    Config::SetDefault(queueDisc + "::MaxSize", QueueSizeValue(QueueSize("100p")));

    NodeContainer sender;
    NodeContainer receiver;
    NodeContainer routers;
    sender.Create(1);
    receiver.Create(1);
    routers.Create(2);

    // Create the point-to-point link helpers
    PointToPointHelper bottleneckLink;
    bottleneckLink.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    bottleneckLink.SetChannelAttribute("Delay", StringValue("10ms"));

    PointToPointHelper edgeLink;
    edgeLink.SetDeviceAttribute("DataRate", StringValue("1000Mbps"));
    edgeLink.SetChannelAttribute("Delay", StringValue("5ms"));

    // Create NetDevice containers
    NetDeviceContainer senderEdge = edgeLink.Install(sender.Get(0), routers.Get(0));
    NetDeviceContainer r1r2 = bottleneckLink.Install(routers.Get(0), routers.Get(1));
    NetDeviceContainer receiverEdge = edgeLink.Install(routers.Get(1), receiver.Get(0));

    // Install Stack
    InternetStackHelper internet;
    internet.Install(sender);
    internet.Install(receiver);
    internet.Install(routers);

    // Configure the root queue discipline
    TrafficControlHelper tch;
    tch.SetRootQueueDisc(queueDisc);

    if (bql)
    {
        tch.SetQueueLimits("ns3::DynamicQueueLimits", "HoldTime", StringValue("1000ms"));
    }

    tch.Install(senderEdge);
    tch.Install(receiverEdge);

    // Assign IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.0.0.0", "255.255.255.0");

    Ipv4InterfaceContainer i1i2 = ipv4.Assign(r1r2);

    ipv4.NewNetwork();
    Ipv4InterfaceContainer is1 = ipv4.Assign(senderEdge);

    ipv4.NewNetwork();
    Ipv4InterfaceContainer ir1 = ipv4.Assign(receiverEdge);

    // Populate routing tables
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Select sender side port
    uint16_t port = 50001;

    // Install application on the sender
    BulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(ir1.GetAddress(1), port));
    source.SetAttribute("MaxBytes", UintegerValue(0));
    ApplicationContainer sourceApps = source.Install(sender.Get(0));
    sourceApps.Start(Seconds(0.1));
    // Hook trace source after application starts
    Simulator::Schedule(Seconds(0.1) + MilliSeconds(1), &TraceCwnd, 0, 0);
    sourceApps.Stop(stopTime);

    // Install application on the receiver
    PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps = sink.Install(receiver.Get(0));
    sinkApps.Start(Seconds(0.0));
    sinkApps.Stop(stopTime);

    // Create a new directory to store the output of the program
    
    std::string dirToSave = "mkdir -p " + dir;
    if (system(dirToSave.c_str()) == -1)
    {
        exit(1);
    }

    std::cout << "Topology complete" << std::endl;

    // The plotting scripts are provided in the following repository, if needed:
    // https://github.com/mohittahiliani/BBR-Validation/
    //
    // Download 'PlotScripts' directory (which is inside ns-3 scripts directory)
    // from the link given above and place it in the ns-3 root directory.
    // Uncomment the following three lines to generate plots for Congestion
    // Window, sender side throughput and queue occupancy on the bottleneck link.
    //
    // system (("cp -R PlotScripts/gnuplotScriptCwnd " + dir).c_str ());
    // system (("cp -R PlotScripts/gnuplotScriptThroughput " + dir).c_str ());
    // system (("cp -R PlotScripts/gnuplotScriptQueueSize " + dir).c_str ());

    // Trace the queue occupancy on the second interface of R1
    tch.Uninstall(routers.Get(0)->GetDevice(1));
    QueueDiscContainer qd;
    qd = tch.Install(routers.Get(0)->GetDevice(1));
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

    // Check for dropped packets using Flow Monitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    // Check if throughput.dat exist, if so, truncate it
    std::ofstream thr2(dir + "/throughput.dat", std::ios::out | std::ios::trunc);
    thr2.close();
    Simulator::Schedule(Seconds(0 + 0.000001), &TraceThroughput, monitor);

    std::cout << "Schedule RTT tracing" << std::endl;

    // Schedule RTT tracing
    Simulator::Schedule(Seconds(0.2) + MilliSeconds(1), &TraceRtt, 0, 0);

    Simulator::Stop(stopTime + TimeStep(1));
    Simulator::Run();
    Simulator::Destroy();

    std::cout << "Simulation finished" << std::endl;

    return 0;
}

