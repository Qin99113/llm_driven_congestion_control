#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/flow-monitor-module.h"
#include <map>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TcpExample");

std::string dir;
uint32_t prev = 0;
Time prevTime = Seconds(0);


// Calculate throughput
void TraceThroughput(Ptr<FlowMonitor> monitor)
{
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    auto itr = stats.begin();
    Time curTime = Now();
    std::ofstream thr(dir + "throughput.dat", std::ios::out | std::ios::app);
    thr << curTime << " "
        << 8 * (itr->second.txBytes - prev) /
               (1000 * 1000 * (curTime.GetSeconds() - prevTime.GetSeconds()))
        << std::endl;
    prevTime = curTime;
    prev = itr->second.txBytes;
    Simulator::Schedule(Seconds(0.2), &TraceThroughput, monitor);
}

// Trace congestion window
void CwndTracer(Ptr<OutputStreamWrapper> stream, uint32_t oldval, uint32_t newval)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() << " " << newval / 1448.0 << std::endl;
}

void TraceCwnd(uint32_t nodeId, uint32_t socketId)
{
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream(dir + "cwnd.dat");
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

int main(int argc, char* argv[])
{
    // Naming the output directory using local system time
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%d-%m-%Y-%I-%M-%S", timeinfo);
    std::string currentTime(buffer);

    // Set the default TCP Congestion Control Protocol to NewReno
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpNewReno"));

    // Command line parser for simulation parameters
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    // Set the resolution for time to nanoseconds
    Time::SetResolution(Time::NS);

    // // Enable logging for PacketSink and OnOffApplication
    // LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    // LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);

    // Create nodes for the network
    NodeContainer nodes;
    nodes.Create(2); // Two nodes (n0: Sender, n1: Receiver)

    // Setup point-to-point connection attributes
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    // Install devices on the nodes
    NetDeviceContainer devices = pointToPoint.Install(nodes);

    // Install the Internet stack on the nodes
    InternetStackHelper stack;
    stack.Install(nodes);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Set up TCP server and client applications
    uint16_t port = 8080;
    Address localAddress(InetSocketAddress(Ipv4Address::GetAny(), port));

    // PacketSink (Receiver) on Node 1
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", localAddress);
    ApplicationContainer serverApps = packetSinkHelper.Install(nodes.Get(1));
    serverApps.Start(Seconds(0.0));
    serverApps.Stop(Seconds(20.0));

    // OnOffApplication (Sender) on Node 0
    OnOffHelper clientHelper("ns3::TcpSocketFactory", Address());
    clientHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    clientHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    clientHelper.SetAttribute("DataRate", DataRateValue(DataRate("5Mbps")));
    clientHelper.SetAttribute("PacketSize", UintegerValue(1024));
    AddressValue remoteAddress(InetSocketAddress(interfaces.GetAddress(1), port));
    clientHelper.SetAttribute("Remote", remoteAddress);

    ApplicationContainer clientApps = clientHelper.Install(nodes.Get(0));
    clientApps.Start(Seconds(0.1));
    clientApps.Stop(Seconds(20.0));


    // Create a new directory to store the output of the program
    dir = "tcp-results/" + currentTime + "/";
    std::string dirToSave = "mkdir -p " + dir;
    if (system(dirToSave.c_str()) == -1)
    {
        exit(1);
    }

    // Schedule congestion window tracing
    Simulator::Schedule(Seconds(0.1) + MilliSeconds(1), &TraceCwnd, 0, 0);

    // Flow Monitor for throughput calculation
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    Simulator::Schedule(Seconds(0.2), &TraceThroughput, monitor);

    // Schedule RTT tracing
    Simulator::Schedule(Seconds(0.1) + MilliSeconds(1), &TraceRtt, 0, 0);

    // Run the simulation
    Simulator::Stop(Seconds(20.1));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
