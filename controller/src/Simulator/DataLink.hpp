#ifndef DATA_LINK_HPP
#define DATA_LINK_HPP

#include <DiagnosticCommands.hpp>
#include <EcuMock.hpp>
#include <ICommunicator.hpp>
#include <Message.hpp>
#include <RequestHandler.hpp>
#include <span>
#include <utility>

constexpr size_t BUFFER_SIZE = 128;
using ByteBuffer = std::array<Byte, BUFFER_SIZE>;

class DataLink {
public:
  int controllerRead();
  bool controllerReadAvailable();
  void controllerWrite(Byte byte);
  void controllerWrite(const std::vector<Byte> &message);

  int ecuRead();
  bool ecuReadAvailable();
  void ecuWrite(Byte byte);
  void ecuWrite(const std::vector<Byte> &message);

  void setOnNewDataForEcu(std::function<void()> onNewDataForEcu) {
    onNewDataForEcu_ = std::move(onNewDataForEcu);
  }

private:
  ByteBuffer controllerBuffer_{};
  ByteBuffer::iterator controllerBegin_ = controllerBuffer_.begin();
  ByteBuffer::iterator controllerEnd_ = controllerBuffer_.begin();
  std::span<Byte> controllerBufferView_{controllerBegin_, controllerEnd_};
  void onControllerBufferChanged_();
  void onControllerBufferReadWhenEmpty_();

  ByteBuffer ecuBuffer_{};
  ByteBuffer::iterator ecuBegin_ = ecuBuffer_.begin();
  ByteBuffer::iterator ecuEnd_ = ecuBuffer_.begin();
  std::span<Byte> ecuBufferView_{ecuBegin_, ecuEnd_};

  void ecuBufferChanged_();
  void newDataForEcu_();
  std::function<void()> onNewDataForEcu_;
};

#endif // DATA_LINK_HPP
