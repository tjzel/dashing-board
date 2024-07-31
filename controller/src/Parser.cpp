#include "parser.h"
#include "requests.h"

int Parser::getByte(const std::string& response, const int pos) {
  const int startIndex = 6 + pos * 3;
  if (startIndex >= response.size()) {
    return -1;
  }
  const auto byte = std::stoi(response.substr(startIndex, 2), nullptr, 16);
  return byte;
}

void Parser::parseIgnore(const std::string& response) { }

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

template<>
double Parser::parse<Requests::ENGINE_LOAD>(const std::string& response) {
  return parsePercentageSingleByte(response);
}

template <>
int Parser::parse<Requests::ENGINE_RPM>(const std::string& response) {
  return parseRPM(response);
}

template <>
int Parser::parse<Requests::VEHICLE_SPEED>(const std::string& response) {
  return parseIdentitySingleByte(response);
}

template<>
double Parser::parse<Requests::THROTTLE_POSITION>(const std::string& response) {
  return parsePercentageSingleByte(response);
}

template<>
int Parser::parse<Requests::UPTIME>(const std::string& response) {
  return parseIdentityDoubleByte(response);
}

template<>
double Parser::parse<Requests::FUEL_LEVEL>(const std::string& response) {
  return parsePercentageSingleByte(response);
}


template<>
double Parser::parse<Requests::ABSOLUTE_LOAD>(const std::string& response) {
  return parsePercentageDoubleByte(response);
}

template<>
double Parser::parse<Requests::RELATIVE_THROTTLE_POSITION>(const std::string& response) {
  return parsePercentageSingleByte(response);
}

template<>
double Parser::parse<Requests::ENGINE_FUEL_RATE>(const std::string& response) {
  return parseEngineFuelRate(response);
}