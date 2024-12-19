#ifndef DIGITAL_SNIFFER_HPP
#define DIGITAL_SNIFFER_HPP

#include <SerialCommunicator.hpp>

class DigitalSniffer {
public:
  void sniff();

  DigitalSniffer(SerialCommunicator &comm, DebugSerialCommunicator &debugComm);

private:
  SerialCommunicator &comm_;
  DebugSerialCommunicator &debugComm_;
};

#endif // DIGITAL_SNIFFER_HPP
