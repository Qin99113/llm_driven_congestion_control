#include "tcp-llm.h"
#include "ns3/log.h"
#include "tcp-socket-state.h"

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
        // NewReno
        // double adder =
        //     static_cast<double>(tcb->m_segmentSize * tcb->m_segmentSize) / tcb->m_cWnd.Get();
        // adder = std::max(1.0, adder);
        // tcb->m_cWnd += static_cast<uint32_t>(adder);
        // NS_LOG_INFO("In CongAvoid, updated to cwnd " << tcb->m_cWnd << " ssthresh "
        //                                              << tcb->m_ssThresh);

        // LLM
        if (tcb->m_lastRtt > trigger_llm_threshold)
        {
            // TODO: Write current parameters to a file for Python script to read

            // TODO: Latency is greater than the threshold. Trigger LLM to generate new parameters
            int res = CallLLM();
            if (res != 0)
            {
                NS_LOG_ERROR("Failed to execute LLM Python script. Exit code: " << res);
            }

            // TODO: Read in json file and change the corresponding socket parameters here
            ns3::TracedValue<u_int32_t> newCwnd = 1000*1448.0;
            ns3::TracedValue<u_int32_t> newSsThresh = 20*1448.0;
            tcb->m_cWnd = newCwnd;
            tcb->m_ssThresh = newSsThresh;
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
    std::string pythonScript = "python generate.py";

    // Execute the Python script and wait for it to complete
    int result = std::system(pythonScript.c_str());
    return result;
}

} // namespace ns3