#pragma
#ifndef TCPLLM_H
#define TCPLLM_H

#include "ns3/tcp-congestion-ops.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"
#include <fstream>
#include <unordered_map>

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