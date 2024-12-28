#include <DigitalSniffer.hpp>

void DigitalSniffer::sniff() {
  const auto byte = comm_.read();
  if (byte > -1) {
    debugComm_.print(byte);
    // if (std::isalpha(static_cast<unsigned char>(byte))) {
    //   debugComm_.print(std::string{' ', static_cast<char>(byte)});
    // }
    // debugComm_.println();
  } else {
    // debugComm_.println("No data");
  }
}

DigitalSniffer::DigitalSniffer(SerialCommunicator &comm, DebugSerialCommunicator &debugComm)
    : comm_(comm), debugComm_(debugComm) {}
