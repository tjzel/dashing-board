#include <DataLink.hpp>
#include <DiagnosticCommands.hpp>
#include <Parser.hpp>
#include <RequestHandler.hpp>
#include <Utils.hpp>

using DiagnosticCommands::ENGINE_RPM;

int DataLink::controllerRead() {
  if (controllerBufferView_.empty()) {
    onControllerBufferReadWhenEmpty_();
  }
  const auto byte = controllerBufferView_.front();
  controllerBegin_++;
  onControllerBufferChanged_();
  return byte;
}

bool DataLink::controllerReadAvailable() { return !controllerBufferView_.empty(); }

void DataLink::controllerWrite(Byte byte) {
  *ecuEnd_++ = byte;

  ecuBufferChanged_();
  onNewDataForEcu_();
}

void DataLink::controllerWrite(const std::vector<Byte> &message) {
  auto *iter = ecuEnd_;
  for (auto byte : message) {
    *iter++ = byte;
  }
  ecuEnd_ = iter;

  ecuBufferChanged_();
  newDataForEcu_();
}

int DataLink::ecuRead() {
  if (ecuBufferView_.empty()) {
    // throw std::runtime_error("ECU should never attempt to read from an empty buffer.");
    return -1;
  }
  const auto byte = ecuBufferView_.front();
  ecuBegin_++;

  ecuBufferChanged_();
  return byte;
}

bool DataLink::ecuReadAvailable() { return !ecuBufferView_.empty(); }

void DataLink::ecuWrite(Byte byte) {
  *controllerEnd_++ = byte;
  onControllerBufferChanged_();
}

void DataLink::ecuWrite(const std::vector<Byte> &message) {
  auto *iter = controllerEnd_;
  for (auto byte : message) {
    *iter++ = byte;
  }
  controllerEnd_ = iter;
  onControllerBufferChanged_();
}

void DataLink::onControllerBufferChanged_() {
  if (controllerBegin_ == controllerEnd_) {
    controllerBegin_ = controllerBuffer_.begin();
    controllerEnd_ = controllerBegin_;
  }
  controllerBufferView_ = std::span<Byte>(controllerBegin_, controllerEnd_);
}

void DataLink::onControllerBufferReadWhenEmpty_() {
  // Do nothing for now.
}

void DataLink::ecuBufferChanged_() {
  if (ecuBegin_ == ecuEnd_) {
    ecuBegin_ = ecuBuffer_.begin();
    ecuEnd_ = ecuBegin_;
  }
  ecuBufferView_ = std::span<Byte>(ecuBegin_, ecuEnd_);
}

void DataLink::newDataForEcu_() { onNewDataForEcu_(); }
