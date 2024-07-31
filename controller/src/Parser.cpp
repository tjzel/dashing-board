#include "Parser.hpp"
#include "Requests.hpp"

std::string Parser::headerPrefix = "41";

int Parser::getByte(const std::string& response, const int pos) {
  const int startIndex = pos * 2;
  if (startIndex + 2 > response.size()) {
    return -1;
  }
  const auto byte = std::stoi(response.substr(startIndex, 2), nullptr, 16);
  return byte;
}

std::string Parser::getData(const std::string& response, const std::string& command) {
  const auto header = headerPrefix + command.substr(2, 2);
  const auto startIndex = response.find(header);
  if (startIndex == std::string::npos) {
    return "";
  }

  return response.substr(startIndex + header.size());
}

void Parser::parseIgnore(const std::string& response) { }

std::vector<int> Parser::parseAvailability(const std::string& response, const std::string& command) {
  const auto data = getData(response, command);
  std::vector<int> result;
  for( auto character : data) {
    const auto value = std::stoi(std::string(1, character), nullptr, 16);
    result.push_back(value);
  }
  return result;
}

double Parser::parsePercentageSingleByte(const std::string& response) {
  const auto A = getByte(response, 0);
  return A / 2.55;
}

double Parser::parsePercentageDoubleByte(const std::string& response) {
  const auto A = getByte(response, 0);
  const auto B = getByte(response, 1);
  return ((A * 256) + B) / 2.55;
}

int Parser::parseIdentitySingleByte(const std::string& response) {
  const auto A = getByte(response, 0);
  return A;
}

int Parser::parseIdentityDoubleByte(const std::string& response) {
  const auto A = getByte(response, 0);
  const auto B = getByte(response, 1);
  return ((A * 256) + B);
}

int Parser::parseRPM(const std::string& response) {
  const auto A = getByte(response, 0);
  const auto B = getByte(response, 1);
  int rpm = ((A * 256) + B) / 4;
  return rpm;
}

int Parser::parseEngineFuelRate(const std::string& response) {
  const auto A = getByte(response, 0);
  const auto B = getByte(response, 1);
  int engineFuelRate = ((A * 256) + B) / 20;
  return engineFuelRate;
}