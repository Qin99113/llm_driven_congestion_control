/*
 * Copyright (c) 2018-20 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Aarti Nandagiri <aarti.nandagiri@gmail.com>
 *          Vivek Jain <jain.vivek.anand@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

// This program simulates the following topology:
//
//               1000 Mbps           10Mbps          1000 Mbps
//  Sender * 3 -------------- R1 -------------- R2 -------------- Receiver
//                  5ms               10ms               5ms
//
// The link between R1 and R2 is a bottleneck link with 10 Mbps. All other
// links are 1000 Mbps.
//
// This program runs by default for 100 seconds and creates a new directory
// called 'bbr-results' in the ns-3 root directory. The program creates one
// sub-directory called 'pcap' in 'bbr-results' directory (if pcap generation
// is enabled) and three .dat files.
//
// (1) 'pcap' sub-directory contains six PCAP files:
//     * bbr-0-0.pcap for the interface on Sender
//     * bbr-1-0.pcap for the interface on Receiver
//     * bbr-2-0.pcap for the first interface on R1
//     * bbr-2-1.pcap for the second interface on R1
//     * bbr-3-0.pcap for the first interface on R2
//     * bbr-3-1.pcap for the second interface on R2
// (2) cwnd.dat file contains congestion window trace for the sender node
// (3) throughput.dat file contains sender side throughput trace
// (4) queueSize.dat file contains queue length trace from the bottleneck link
//
// BBR algorithm enters PROBE_RTT phase in every 10 seconds. The congestion
// window is fixed to 4 segments in this phase with a goal to achieve a better
// estimate of minimum RTT (because queue at the bottleneck link tends to drain
// when the congestion window is reduced to 4 segments).
//
// The congestion window and queue occupancy traces output by this program show
// periodic drops every 10 seconds when BBR algorithm is in PROBE_RTT phase.

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
// uint32_t prev = 0;
Time prevTime = Seconds(0);

void InitializePrevFlowTxBytes(Ptr<FlowMonitor> monitor)
{
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    for (const auto& stat : stats)
    {
        std::cout << "Hey I got here" << std::endl;
        uint32_t flowId = stat.first;
        prevFlowTxBytes[flowId] = 0; // Initialize to 0
    }
}

// // Calculate throughput
// static void
// TraceThroughput(Ptr<FlowMonitor> monitor, Ptr<Ipv4FlowClassifier> classifier)
// {
//     // FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
//     // auto itr = stats.begin();
//     // Time curTime = Now();
//     // std::ofstream thr(dir + "/throughput.dat", std::ios::out | std::ios::app);
//     // thr << curTime << " "
//     //     << 8 * (itr->second.txBytes - prev) /
//     //            (1000 * 1000 * (curTime.GetSeconds() - prevTime.GetSeconds()))
//     //     << std::endl;
//     // prevTime = curTime;
//     // prev = itr->second.txBytes;
//     // Simulator::Schedule(Seconds(0.2), &TraceThroughput, monitor);

//     FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
//     Time curTime = Now();
//     std::ofstream thr(dir + "/throughput.dat", std::ios::out | std::ios::app);

//     for (const auto& stat : stats)
//     {
//         uint32_t flowId = stat.first; // Flow ID

//         // Get source and destination addresses using the classifier
//         Ipv4FlowClassifier::FiveTuple fiveTuple = classifier->FindFlow(flowId);
//         std::cout << "Flow ID: " << flowId << " Source: ";
//         fiveTuple.sourceAddress.Print(std::cout);
//         std::cout << " Destination: ";
//         fiveTuple.destinationAddress.Print(std::cout);
//         std::cout << std::endl;
        
//         const FlowMonitor::FlowStats& flowStats = stat.second;
//         thr << "Flow " << flowId << " at time " << curTime.GetSeconds() << " s: "
//             << 8 * (flowStats.txBytes - prevFlowTxBytes[flowId]) /
//                    (1000 * 1000 * (curTime.GetSeconds() - prevTime.GetSeconds()))
//             << " Mbps" << std::endl;

//         // Update previous values for this flow
//         prevFlowTxBytes[flowId] = flowStats.txBytes;
//     }
//     prevTime = curTime;
//     Simulator::Schedule(Seconds(0.2), &TraceThroughput, monitor, classifier);
// }

// Calculate throughput
static void
TraceThroughput(Ptr<FlowMonitor> monitor)
{
    // FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    // auto itr = stats.begin();
    // Time curTime = Now();
    // std::ofstream thr(dir + "/throughput.dat", std::ios::out | std::ios::app);
    // thr << curTime << " "
    //     << 8 * (itr->second.txBytes - prev) /
    //            (1000 * 1000 * (curTime.GetSeconds() - prevTime.GetSeconds()))
    //     << std::endl;
    // prevTime = curTime;
    // prev = itr->second.txBytes;
    // Simulator::Schedule(Seconds(0.2), &TraceThroughput, monitor);

    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    Time curTime = Now();
    std::ofstream thr(dir + "/throughput.dat", std::ios::out | std::ios::app);

    for (const auto& stat : stats)
    {
        uint32_t flowId = stat.first; // Flow ID
        // Only consider flows 1, 2, and 3
        if (0 < flowId && flowId < 4)
        {
            const FlowMonitor::FlowStats& flowStats = stat.second;
            thr << "Flow " << flowId << " at time " << curTime.GetSeconds() << " s: "
                << 8 * (flowStats.txBytes - prevFlowTxBytes[flowId]) /
                        (1000 * 1000 * (curTime.GetSeconds() - prevTime.GetSeconds()))
                << " Mbps" << std::endl;

            // Update previous values for this flow
            prevFlowTxBytes[flowId] = flowStats.txBytes;
        }
    }
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

    std::string tcpTypeId = "TcpNewReno";
    std::string queueDisc = "FifoQueueDisc";
    uint32_t delAckCount = 2;
    bool bql = true;
    bool enablePcap = false;
    Time stopTime = Seconds(20);

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

    // Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::" + tcpTypeId));
    Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(4194304));
    Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(6291456));
    Config::SetDefault("ns3::TcpSocket::InitialCwnd", UintegerValue(10));
    Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(delAckCount));
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1448));
    Config::SetDefault("ns3::DropTailQueue<Packet>::MaxSize", QueueSizeValue(QueueSize("1p")));
    Config::SetDefault(queueDisc + "::MaxSize", QueueSizeValue(QueueSize("100p")));

    // Modified
    // Sender: 0, 1, 2, Receiver: 3, Router: 4, 5
    NodeContainer senders;
    NodeContainer receiver;
    NodeContainer routers;
    senders.Create(3);
    // uint32_t nNodes = senders.GetN();
    // for (uint32_t i = 0; i < nNodes; ++i)
    // {
    //     Ptr<Node> p = senders.Get(i);
    //     std::cout << "Node " << p->GetId() << std::endl;  // some Node method
    // }

    receiver.Create(1);
    // uint32_t nNodes1 = receiver.GetN();
    // for (uint32_t i = 0; i < nNodes1; ++i)
    // {
    //     Ptr<Node> p = receiver.Get(i);
    //     std::cout << "Node " << p->GetId() << std::endl;  // some Node method
    // }

    routers.Create(2);
    // uint32_t nNodes2 = routers.GetN();
    // for (uint32_t i = 0; i < nNodes2; ++i)
    // {
    //     Ptr<Node> p = routers.Get(i);
    //     std::cout << "Node " << p->GetId() << std::endl;  // some Node method
    // }

    // Create the point-to-point link helpers
    PointToPointHelper bottleneckLink;
    bottleneckLink.SetDeviceAttribute("DataRate", StringValue("1000Mbps"));
    bottleneckLink.SetChannelAttribute("Delay", StringValue("10ms"));

    PointToPointHelper edgeLink;
    edgeLink.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    edgeLink.SetChannelAttribute("Delay", StringValue("5ms"));

    PointToPointHelper edgeLink1;
    edgeLink1.SetDeviceAttribute("DataRate", StringValue("1000Mbps"));
    edgeLink1.SetChannelAttribute("Delay", StringValue("5ms"));

    // Create NetDevice containers
    // Modifeid
    NetDeviceContainer sender0Edge = edgeLink.Install(senders.Get(0), routers.Get(0));
    NetDeviceContainer sender1Edge = edgeLink.Install(senders.Get(1), routers.Get(0));
    NetDeviceContainer sender2Edge = edgeLink.Install(senders.Get(2), routers.Get(0));
    NetDeviceContainer r1r2 = bottleneckLink.Install(routers.Get(0), routers.Get(1));
    NetDeviceContainer receiverEdge = edgeLink1.Install(routers.Get(1), receiver.Get(0));

    // Install Stack
    // Modifeid
    InternetStackHelper internet;
    internet.Install(senders);
    internet.Install(receiver);
    internet.Install(routers);

    // Configure the root queue discipline
    TrafficControlHelper tch;
    tch.SetRootQueueDisc(queueDisc);

    if (bql)
    {
        tch.SetQueueLimits("ns3::DynamicQueueLimits", "HoldTime", StringValue("1000ms"));
    }

    // Modifeid
    tch.Install(sender0Edge);
    tch.Install(sender1Edge);
    tch.Install(sender2Edge);
    tch.Install(receiverEdge);

    // Assign IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer i1i2 = ipv4.Assign(r1r2);

    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer is0 = ipv4.Assign(sender0Edge);

    ipv4.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer is1 = ipv4.Assign(sender1Edge);

    ipv4.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer is2 = ipv4.Assign(sender2Edge);

    ipv4.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer ir1 = ipv4.Assign(receiverEdge);

    // Populate routing tables
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Select sender side port
    uint16_t port_0 = 50001;

    for (uint16_t i = 0; i < 3; i++)
    {
        port_0 = port_0 + i;
        // Install application on the sender
        BulkSendHelper source0("ns3::TcpSocketFactory", InetSocketAddress(ir1.GetAddress(1), port_0));
        source0.SetAttribute("MaxBytes", UintegerValue(0));
        ApplicationContainer sourceApps0 = source0.Install(senders.Get(i));
        sourceApps0.Start(Seconds(0.1));
        // Hook trace source after application starts // tracing sender 0
        Simulator::Schedule(Seconds(0.1) + MilliSeconds(1), &TraceCwnd, i, 0);
        sourceApps0.Stop(stopTime);

        // Install application on the receiver
        PacketSinkHelper sink0("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port_0));
        ApplicationContainer sinkApps0 = sink0.Install(receiver.Get(0));
        sinkApps0.Start(Seconds(0.0));
        sinkApps0.Stop(stopTime);
    }

    // Create a new directory to store the output of the program
    dir = "multi_sender_results/";
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
        bottleneckLink.EnablePcapAll(dir + "/pcap/bbr", true);
    }

    // Check for dropped packets using Flow Monitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    // Check if throughput.dat exist, if so, truncate it
    std::ofstream thr2(dir + "/throughput.dat", std::ios::out | std::ios::trunc);
    thr2.close();

    // Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());

    Simulator::Schedule(Seconds(0 + 0.000001), &TraceThroughput, monitor);

    std::cout << "Schedule RTT tracing" << std::endl;

    // Schedule RTT tracing
    Simulator::Schedule(Seconds(0.2) + MilliSeconds(1), &TraceRtt, 0, 0);
    Simulator::Schedule(Seconds(0.2) + MilliSeconds(1), &TraceRtt, 1, 0);
    Simulator::Schedule(Seconds(0.2) + MilliSeconds(1), &TraceRtt, 2, 0);

    Simulator::Stop(stopTime + TimeStep(1));
    Simulator::Run();
    Simulator::Destroy();

    std::cout << "Simulation finished" << std::endl;

    return 0;
}

