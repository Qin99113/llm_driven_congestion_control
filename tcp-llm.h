#ifndef TCPLLM_H
#define TCPLLM_H

#include "tcp-congestion-ops.h"
#include "ns3/core-module.h"
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
  // Store the file path for throughput.dat
  std::string m_throughputFilePath;
  ns3::Time trigger_llm_threshold;
  std::vector<std::string> last_n_rtt;
  std::unordered_map<std::string, std::string> ParseLLMOutput();
  int64_t last_trigger_time;
  int64_t wait_threshold;
  std::string topology_description;

};

}

#endif