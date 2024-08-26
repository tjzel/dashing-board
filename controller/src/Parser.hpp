#ifndef PARSER_HPP
#define PARSER_HPP
#include <DiagnosticCommands.hpp>
#include <_types/_uint8_t.h>
#include <span>
#include <string>
#include <type_traits>

template <typename TCommand, typename = void> struct Parser {
  static auto parse(std::span<const uint8_t>) -> typename TCommand::Type {
    return -1;
  }
};

template <DiagnosticCommands::Command TCommand>
struct Parser<
    TCommand,
    std::enable_if_t<std::is_same_v<typename TCommand::ParsingAlgorithm,
                                    ParsingFormulas::CommandAvailability>>> {
  static auto parse(std::span<const uint8_t> data) -> typename TCommand::Type {
    std::unordered_map<uint8_t, bool> availability;
    const auto commandOffset = TCommand::command + 1;

    for (auto byte : data) {
      const uint8_t byteOffset = byte * 8;
      for (int i = 0; i < 8; i++) {
        const auto bit = byte & (1 << (7 - i));
        const auto keyOffset = commandOffset + byteOffset + i;
        availability[keyOffset + i] = bit;
      }
    }
    return availability;
  }
};

template <DiagnosticCommands::Command TCommand>
struct Parser<TCommand,
              std::enable_if_t<std::is_same_v<
                  typename TCommand::ParsingAlgorithm,
                  ParsingFormulas::Identity<
                      TCommand::ParsingAlgorithm::byteCount,
                      typename TCommand::ParsingAlgorithm::ValueType>>>> {
  static auto parse(std::span<const uint8_t> data) -> typename TCommand::Type {
    static_assert(data.size() == TCommand::ParsingAlgorithm::byteCount);
    typename TCommand::ParsingAlgorithm::ValueType result = 0;
    for (auto byte : data) {
      result = result * 256 + byte;
    }
    return result;
  }
};

template <DiagnosticCommands::Command TCommand>
struct Parser<TCommand, std::enable_if_t<std::is_same_v<
                            typename TCommand::ParsingAlgorithm,
                            ParsingFormulas::MultiplyBy<
                                TCommand::ParsingAlgorithm::byteCount,
                                typename TCommand::ParsingAlgorithm::ValueType,
                                TCommand::ParsingAlgorithm::Multiplier>>>> {
  static auto parse(std::span<const uint8_t> data) -> typename TCommand::Type {
    static_assert(data.size() == TCommand::ParsingAlgorithm::byteCount);
    typename TCommand::ParsingAlgorithm::ValueType result = 0;
    for (auto byte : data) {
      result = result * 256 + byte;
    }
    return result / TCommand::ParsingAlgorithm::Multiplier;
  }
};

template <DiagnosticCommands::Command TCommand>
struct Parser<TCommand, std::enable_if_t<std::is_same_v<
                            typename TCommand::ParsingAlgorithm,
                            ParsingFormulas::DivideBy<
                                TCommand::ParsingAlgorithm::byteCount,
                                typename TCommand::ParsingAlgorithm::ValueType,
                                TCommand::ParsingAlgorithm::Nominator,
                                TCommand::ParsingAlgorithm::Denominator>>>> {
  static auto parse(std::span<const uint8_t> data) -> typename TCommand::Type {
    static_assert(data.size() == TCommand::ParsingAlgorithm::byteCount);
    typename TCommand::ParsingAlgorithm::ValueType result = 0;
    for (auto byte : data) {
      result = result * 256 + byte;
    }
    return result / (TCommand::ParsingAlgorithm::Nominator /
                     TCommand::ParsingAlgorithm::Denominator);
  }
};

template <DiagnosticCommands::Command TCommand>
struct Parser<
    TCommand,
    std::enable_if_t<std::is_same_v<
        typename TCommand::ParsingAlgorithm,
        ParsingFormulas::Percentage<TCommand::ParsingAlgorithm::byteCount>>>> {
  static auto parse(std::span<const uint8_t> data) -> typename TCommand::Type {
    static_assert(data.size() == TCommand::ParsingAlgorithm::byteCount);
    typename TCommand::ParsingAlgorithm::ValueType result = 0;
    for (auto byte : data) {
      result = result * 256 + byte;
    }
    return result / (TCommand::ParsingAlgorithm::Nominator /
                     TCommand::ParsingAlgorithm::Denominator);
  }
};

#endif // PARSER_HPP
