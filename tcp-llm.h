#ifndef TCPLLM_H
#define TCPLLM_H

#include "tcp-congestion-ops.h"

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
  ns3::Time trigger_llm_threshold;
};

}

#endif