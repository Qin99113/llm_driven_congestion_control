// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

// Define the logging component for this simulation
NS_LOG_COMPONENT_DEFINE("TcpExample");

int main(int argc, char* argv[])
{
    // Set the default TCP Congestion Control Protocol to be used in the simulation
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpNewReno"));

    // Command line parser for simulation parameters
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    // Set the resolution for time to nanoseconds
    Time::SetResolution(Time::NS);
    
    // Enable logging for specific components
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);// Enable logging for the PacketSink(Packet Receiver) application
    LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);// Enable logging for the OnOffApplication(Packet Sender) application

    // Create a container for the nodes (devices)
    NodeContainer nodes;
    nodes.Create(2); // Create 2 nodes

    // Setup point-to-point connection attributes
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps")); // Set data rate
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms")); // Set delay

    // Install devices on the nodes according to the point-to-point configuration
    NetDeviceContainer devices = pointToPoint.Install(nodes);

    // Install the Internet stack (TCP/IP) on the created nodes
    InternetStackHelper stack;
    stack.Install(nodes);

    // Set up IP addresses for the nodes
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0"); // Define the network address and subnet mask

    // Assign IP addresses to the devices
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Set up TCP server and client
    uint16_t port = 8080; // Define the server port
    Address localAddress(InetSocketAddress(Ipv4Address::GetAny(), port)); // Create an address to listen on the specified port
    
    // Create a PacketSink application (server) that will receive packets
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", localAddress);
    ApplicationContainer serverApps = packetSinkHelper.Install(nodes.Get(1)); // Install on the second node
    serverApps.Start(Seconds(1.0)); // Start the server at 1 second
    serverApps.Stop(Seconds(10.0)); // Stop the server at 10 seconds

    // Create an OnOff application (client) that will send packets
    OnOffHelper clientHelper("ns3::TcpSocketFactory", Address());
    clientHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]")); // Set the 'on' time to 1 second
    clientHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]")); // Set the 'off' time to 0 seconds (always 'on')
    clientHelper.SetAttribute("DataRate", DataRateValue(DataRate("5Mbps"))); // Set the data rate for the client
    clientHelper.SetAttribute("PacketSize", UintegerValue(1024)); // Set the size of packets to send (1024 bytes)
    
    // Set the address of the receiving server for the client application
    AddressValue remoteAddress(InetSocketAddress(interfaces.GetAddress(1), port));
    clientHelper.SetAttribute("Remote", remoteAddress);

    // Install the client application on the first node
    ApplicationContainer clientApps = clientHelper.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0)); // Start the client at 2 seconds
    clientApps.Stop(Seconds(10.0)); // Stop the client at 10 seconds

    // Run the simulation
    Simulator::Run();
    // Clean up and destroy the simulator
    Simulator::Destroy();
    return 0; // Return success
}
