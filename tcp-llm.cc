#include "tcp-llm.h"
#include "ns3/log.h"
#include "tcp-socket-state.h"
#include "ns3/traced-value.h"
#include "ns3/string.h"
#include "ns3/attribute-helper.h"


namespace ns3 {

// Libra
NS_LOG_COMPONENT_DEFINE ("TcpLlm");
NS_OBJECT_ENSURE_REGISTERED (TcpLlm);

TypeId
TcpLlm::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpLlm")
    .SetParent<TcpNewReno> ()
    .SetGroupName ("Internet")
    .AddConstructor<TcpLlm> ()
    .AddAttribute("ThroughputFilePath", 
                    "The file path for throughput results.",
                    StringValue("./throughput.dat"), 
                    MakeStringAccessor(&TcpLlm::m_throughputFilePath), 
                    MakeStringChecker()); 
  ;
  return tid;
}

TcpLlm::TcpLlm (void) 
: TcpNewReno (),
  trigger_llm_threshold(Seconds(0.1))
{
  NS_LOG_FUNCTION (this);
}

TcpLlm::TcpLlm (const TcpLlm& sock)
  : TcpNewReno (sock),
    trigger_llm_threshold(sock.trigger_llm_threshold)
{
  NS_LOG_FUNCTION (this);
}

TcpLlm::~TcpLlm (void)
{
}

uint32_t
TcpLlm::SlowStart (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this << tcb << segmentsAcked);

    if (segmentsAcked >= 1)
    {
        tcb->m_cWnd += tcb->m_segmentSize;
        NS_LOG_INFO("In SlowStart, updated to cwnd " << tcb->m_cWnd << " ssthresh "
                                                        << tcb->m_ssThresh);
        return segmentsAcked - 1;
    }

    return 0;
}

void
TcpLlm::CongestionAvoidance(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this << tcb << segmentsAcked);

    if (segmentsAcked > 0)
    {
        // LLM
        if (tcb->m_lastRtt > trigger_llm_threshold)
        {
            // TODO: Write current parameters to a file for Python script to read
            std::string filename = "./scratch/history.txt";
            std::ofstream outputFile(filename);

            if (outputFile.is_open()) 
            {
                // Topology
                outputFile << "topology description: Star topology with 5 nodes, each connected to a central hub.\n";
                // Base algorithm
                outputFile << "base algorithm: TCP NewReno\n";
                // Current cwnd
                outputFile << "cwnd: " << tcb->m_cWnd << "\n";
                // Current ssthreshold
                outputFile << "ssthreshold: " << tcb->m_ssThresh << "\n";
                // Current RTT
                outputFile << "RTT: " << tcb->m_lastRtt << "\n";
                // Current throughput
                // Read in from file
                std::string throughput = ReadThroughput();
                outputFile << "throughput: " << throughput << " kbps\n"; // TODO
                // Current trigger latency llm threshold
                outputFile << "current trigger latency llm threshold: " << trigger_llm_threshold << " ms\n";
                // Close the file
                outputFile.close();
                // std::cout << "File written successfully to " << filename << std::endl;
            }
            else 
            {
                std::cerr << "Error: Unable to open file for writing." << std::endl;
            }

            // TODO: Latency is greater than the threshold. Trigger LLM to generate new parameters
            int res = CallLLM();
            if (res != 0)
            {
                NS_LOG_ERROR("Failed to execute LLM Python script. Exit code: " << res);
            }

            // Read in txt file and change the corresponding socket parameters here
            std::unordered_map<std::string, std::string> parsedData = ParseLLMOutput();
            
            u_int32_t new_cwnd_int;
            std::istringstream iss(parsedData["CWND"]);
            iss >> new_cwnd_int;

            u_int32_t new_ss_int;
            std::istringstream isss(parsedData["SSThreshold"]);
            isss >> new_ss_int;

            ns3::Time new_thresh;
            std::istringstream issss(parsedData["Trigger_latency_llm_threshold"]);
            issss >> new_thresh;


            ns3::TracedValue<u_int32_t> newCwnd = new_cwnd_int;
            ns3::TracedValue<u_int32_t> newSsThresh = new_ss_int;

            tcb->m_cWnd = newCwnd;
            tcb->m_ssThresh = newSsThresh;
            // trigger_llm_threshold = new_thresh;
        }
        else
        {
            // Regular TcpNewReno
            double adder =
            static_cast<double>(tcb->m_segmentSize * tcb->m_segmentSize) / tcb->m_cWnd.Get();
            adder = std::max(1.0, adder);
            tcb->m_cWnd += static_cast<uint32_t>(adder);
            NS_LOG_INFO("In CongAvoid, updated to cwnd " << tcb->m_cWnd << " ssthresh "
                                                            << tcb->m_ssThresh);
        }
    }
}

void
TcpLlm::IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this << tcb << segmentsAcked);

    if (tcb->m_cWnd < tcb->m_ssThresh)
    {
        segmentsAcked = SlowStart(tcb, segmentsAcked);
    }

    if (tcb->m_cWnd >= tcb->m_ssThresh)
    {
        CongestionAvoidance(tcb, segmentsAcked);
    }
}

std::string
TcpLlm::GetName () const
{
  return "TcpLlm";
}

uint32_t
TcpLlm::GetSsThresh (Ptr<const TcpSocketState> state,
                         uint32_t bytesInFlight)
{
    NS_LOG_FUNCTION(this << state << bytesInFlight);

    return std::max(2 * state->m_segmentSize, bytesInFlight / 2);
}

Ptr<TcpCongestionOps>
TcpLlm::Fork ()
{
  return CopyObject<TcpLlm> (this);
}

int TcpLlm::CallLLM()
{
    // Define the Python script and output file
    std::string pythonScript = "python ./scratch/0_1_prompt_tests.py";

    // Execute the Python script and wait for it to complete
    int result = std::system(pythonScript.c_str());
    return result;
}


std::string TcpLlm::ReadThroughput()
{
    // Read the throughput from the file
    // std::string filename = "./llm-results_unique/throughput.dat";
    // std::string throughput;
    // std::ifstream inputFile(filename, std::ios::in | std::ios::binary);
    std::string throughput;
    std::ifstream inputFile(m_throughputFilePath, std::ios::in | std::ios::binary);
    if (inputFile.is_open()) 
    {
        std::string lastLine;

        // Seek to the end of the file
        inputFile.seekg(0, std::ios::end);
        size_t fileSize = inputFile.tellg();

        // Traverse backwards to find the last newline
        for (size_t i = 1; i <= fileSize; ++i) 
        {
            inputFile.seekg(-i, std::ios::end);
            char ch;
            inputFile.get(ch);
            if (ch == '\n' && i != 1) {
                // Found the last newline, read the last line
                std::getline(inputFile, lastLine);
                break;
            }
        }

        // Handle files with a single line (no newline character)
        if (lastLine.empty()) 
        {
            inputFile.clear();
            inputFile.seekg(0, std::ios::beg);
            std::getline(inputFile, lastLine);
        }

        inputFile.close();

        if (!lastLine.empty()) 
        {
            size_t spacePos = lastLine.find(' ');
            if (spacePos != std::string::npos) 
            {
                // Only get the throughput value
                throughput = lastLine.substr(spacePos + 1);
            }
        } 
        else 
        {
            std::cout << "File is empty or contains no valid lines." << std::endl;
        }
    } 
    else 
    {
        std::cerr << "Error: Unable to open file for reading." << std::endl;
    }

    return throughput;
}



std::unordered_map<std::string, std::string> TcpLlm::ParseLLMOutput()
{
    std::string filename = "./scratch/llm_output.txt";
    std::unordered_map<std::string, std::string> parsedData;

    std::ifstream inputFile(filename);

    if (inputFile.is_open()) 
    {       
        std::string line;

        while (std::getline(inputFile, line)) 
        {
            // Find the position of the colon
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) 
            {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 2); // Skip ": " (2 characters)
                
                parsedData[key] = value;
            }
        }

        inputFile.close();
    } 
    else 
    {
        std::cerr << "Error: Unable to open file for reading." << std::endl;
    }  
    return parsedData; 
}


} // namespace ns3