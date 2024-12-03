#ifndef TCPLLM_H
#define TCPLLM_H

#include "tcp-congestion-ops.h"
#include <fstream>

namespace ns3 {

class TcpLlm : public TcpNewReno
{
public:

  static TypeId GetTypeId (void);

  TcpLlm ();

  TcpLlm (const TcpLlm& sock);

  ~TcpLlm ();

  virtual std::string GetName () const;

  virtual void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
  virtual uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb,
                                uint32_t bytesInFlight);
  virtual Ptr<TcpCongestionOps> Fork ();

protected:
  virtual uint32_t SlowStart (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
  // Just need one CongestionAvoidance method
  virtual void CongestionAvoidance(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);

private:
  // Call llm
  int CallLLM();
  // Helper function to read throughput.dat
  std::string ReadThroughput();
  ns3::Time trigger_llm_threshold;
  std::vector<std::string> last_n_rtt;
  std::unordered_map<std::string, std::string> ParseLLMOutput();

};

}

#endif