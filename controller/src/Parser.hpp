#ifndef PARSER_HPP
#define PARSER_HPP
#include <string>
#include <Requests.hpp>

class Parser {
public:
  template <class TCommand>
  static typename TCommand::Type parse(const std::string& response);
    static std::vector<int> parseAvailability(const std::string& response, const std::string& command);

private:
  template <class TCommand>
  static typename TCommand::Type parseData(const std::string& data);

  static std::string getData(const std::string& response, const std::string& command);
  static int getByte(const std::string& data, int pos);
  static std::string headerPrefix;

  static void parseIgnore(const std::string& data);
  static double parsePercentageSingleByte(const std::string& data);
  static double parsePercentageDoubleByte(const std::string& data);
  static int parseIdentitySingleByte(const std::string& data);
  static int parseIdentityDoubleByte(const std::string& data);
  static int parseRPM(const std::string& data);
  static int parseEngineFuelRate(const std::string& data);
};

template <class TCommand>
typename TCommand::Type Parser::parseData(const std::string& data) {
  return -1;
}

template <class TCommand>
typename TCommand::Type Parser::parse(const std::string& response) {
  const auto data = getData(response, TCommand::toString());
  return Parser::parseData<TCommand>(data);
}

template<>
inline double Parser::parseData<Requests::ENGINE_LOAD>(const std::string& data) {
  return parsePercentageSingleByte(data);
}

template <>
inline int Parser::parseData<Requests::ENGINE_RPM>(const std::string& data) {
  return parseRPM(data);
}

template <>
inline int Parser::parseData<Requests::VEHICLE_SPEED>(const std::string& data) {
  return parseIdentitySingleByte(data);
}

template<>
inline double Parser::parseData<Requests::THROTTLE_POSITION>(const std::string& data) {
  return parsePercentageSingleByte(data);
}

template<>
inline int Parser::parseData<Requests::UPTIME>(const std::string& data) {
  return parseIdentityDoubleByte(data);
}

template<>
inline double Parser::parseData<Requests::FUEL_LEVEL>(const std::string& data) {
  return parsePercentageSingleByte(data);
}


template<>
inline double Parser::parseData<Requests::ABSOLUTE_LOAD>(const std::string& data) {
  return parsePercentageDoubleByte(data);
}

template<>
inline double Parser::parseData<Requests::RELATIVE_THROTTLE_POSITION>(const std::string& data) {
  return parsePercentageSingleByte(data);
}

template<>
inline double Parser::parseData<Requests::ENGINE_FUEL_RATE>(const std::string& data) {
  return parseEngineFuelRate(data);
}
#endif // PARSER_HPP
