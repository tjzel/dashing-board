#ifndef COMMUNICATORS_HPP
#define COMMUNICATORS_HPP

#include "DataLink.hpp"
#include <DiagnosticCommands.hpp>
#include <EcuMock.hpp>
#include <ICommunicator.hpp>
#include <Message.hpp>
#include <RequestHandler.hpp>
#include <string>

class RequestHandlerDataLinkCommunicator {
public:
  int read();
  bool available();

  void write(Byte byte);
  void write(const std::vector<Byte> &message);

  void init();
  void setOnNewData(std::function<void()> onNewData);

  explicit RequestHandlerDataLinkCommunicator(DataLink &dataLink);

private:
  DataLink &dataLink_;
};

static_assert(ICommunicator<RequestHandlerDataLinkCommunicator>);

class EcuMockDataLinkCommunicator {
public:
  int read();
  bool available();

  void write(Byte byte);
  void write(const std::vector<Byte> &message);

  void init();

  void setOnNewData(std::function<void()> onNewData);

  explicit EcuMockDataLinkCommunicator(DataLink &dataLinkMock);

private:
  DataLink &dataLink_;
};

static_assert(ICommunicator<EcuMockDataLinkCommunicator>);

class StdioDebugCommunicator {
public:
  static void print(Byte byte);
  static void print(const std::string &str);
  static void println(Byte byte);
  static void println(const std::string &str);
  static void println();
};

static_assert(IDebugCommunicator<StdioDebugCommunicator>);

#endif // COMMUNICATORS_HPP
