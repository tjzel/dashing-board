#ifndef PARSER_H
#define PARSER_H
#include <string>

class Parser {
public:
  template <class TCommand>
  static typename TCommand::Type parse(const std::string& response) {
    return -1;
  }

private:
  static int getByte(const std::string& response, int pos);

  static void parseIgnore(const std::string& response);
  static double parsePercentageSingleByte(const std::string& response);
  static double parsePercentageDoubleByte(const std::string& response);
  static int parseIdentitySingleByte(const std::string& response);
  static int parseIdentityDoubleByte(const std::string& response);
  static int parseRPM(const std::string& response);
  static int parseEngineFuelRate(const std::string& response);
};
#endif // PARSER_H
