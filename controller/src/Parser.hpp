#ifndef PARSER_HPP
#define PARSER_HPP
#include <DiagnosticCommands.hpp>
#include <_types/_uint8_t.h>
#include <span>
#include <string>
#include <type_traits>

template <DiagnosticCommands::Command TCommand> struct Parser;

static_assert(
    std::is_same_v<
        typename DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::ParsingFormula,
        ParsingFormulas::CommandAvailability>);

template <DiagnosticCommands::Command TCommand>
  requires std::is_same_v<typename TCommand::ParsingFormula,
                          ParsingFormulas::CommandAvailability>

struct Parser<TCommand> {
  static auto parse(std::span<const uint8_t> data) ->
      typename TCommand::ParsingFormula::ValueType {
    std::map<CommandLiteral, bool> availability;
    const auto commandOffset = TCommand::command + 1;

    for (auto byte : data) {
      const uint8_t byteOffset = byte * 8;
      for (int i = 0; i < 8; i++) {
        const bool bit = byte & (1 << (7 - i));
        const uint8_t keyOffset = commandOffset + byteOffset + i;
        const CommandLiteral key = {TCommand::prefix, keyOffset};
        availability[key] = bit;
      }
    }
    return availability;
  }
};

template <DiagnosticCommands::Command TCommand>
  requires std::is_same_v<
      typename TCommand::ParsingFormula,
      ParsingFormulas::Identity<TCommand::ParsingFormula::byteCount,
                                typename TCommand::ParsingFormula::ValueType>>
struct Parser<TCommand
              // ,
              //               std::enable_if_t<std::is_same_v<
              //                   typename TCommand::ParsingFormula,
              //                   ParsingFormulas::Identity<
              //                       TCommand::ParsingFormula::byteCount,
              //                       typename
              //                       TCommand::ParsingFormula::ValueType>>>
              > {
  static auto parse(std::span<const uint8_t> data) ->
      typename TCommand::ParsingFormula::ValueType {
    static_assert(data.size() == TCommand::ParsingFormula::byteCount);
    typename TCommand::ParsingFormula::ValueType result = 0;
    for (auto byte : data) {
      result = result * 256 + byte;
    }
    return result;
  }
};

template <DiagnosticCommands::Command TCommand>
  requires std::is_same_v<
      typename TCommand::ParsingFormula,
      ParsingFormulas::MultiplyBy<TCommand::ParsingFormula::byteCount,
                                  typename TCommand::ParsingFormula::ValueType,
                                  TCommand::ParsingFormula::Multiplier>>
struct Parser<
    TCommand
    // , std::enable_if_t<std::is_same_v<
    //                             typename TCommand::ParsingFormula,
    //                             ParsingFormulas::MultiplyBy<
    //                                 TCommand::ParsingFormula::byteCount,
    //                                 typename
    //                                 TCommand::ParsingFormula::ValueType,
    //                                 TCommand::ParsingFormula::Multiplier>>>
    > {
  static auto parse(std::span<const uint8_t> data) ->
      typename TCommand::ParsingFormula::ValueType {
    static_assert(data.size() == TCommand::ParsingFormula::byteCount);
    typename TCommand::ParsingFormula::ValueType result = 0;
    for (auto byte : data) {
      result = result * 256 + byte;
    }
    return result / TCommand::ParsingFormula::Multiplier;
  }
};

template <DiagnosticCommands::Command TCommand>
  requires std::is_same_v<
      typename TCommand::ParsingFormula,
      ParsingFormulas::DivideBy<TCommand::ParsingFormula::byteCount,
                                typename TCommand::ParsingFormula::ValueType,
                                TCommand::ParsingFormula::Nominator,
                                TCommand::ParsingFormula::Denominator>>
struct Parser<
    TCommand
    // , std::enable_if_t<std::is_same_v<
    //                             typename TCommand::ParsingFormula,
    //                             ParsingFormulas::DivideBy<
    //                                 TCommand::ParsingFormula::byteCount,
    //                                 typename
    //                                 TCommand::ParsingFormula::ValueType,
    //                                 TCommand::ParsingFormula::Nominator,
    //                                 TCommand::ParsingFormula::Denominator>>>
    > {
  static auto parse(std::span<const uint8_t> data) ->
      typename TCommand::ParsingFormula::ValueType {
    static_assert(data.size() == TCommand::ParsingFormula::byteCount);
    typename TCommand::ParsingFormula::ValueType result = 0;
    for (auto byte : data) {
      result = result * 256 + byte;
    }
    return result / (TCommand::ParsingFormula::Nominator /
                     TCommand::ParsingFormula::Denominator);
  }
};

template <DiagnosticCommands::Command TCommand>
  requires std::is_same_v<
      typename TCommand::ParsingFormula,
      ParsingFormulas::Percentage<TCommand::ParsingFormula::byteCount>>
struct Parser<
    TCommand
    // ,
    // std::enable_if_t<std::is_same_v<
    //     typename TCommand::ParsingFormula,
    //     ParsingFormulas::Percentage<TCommand::ParsingFormula::byteCount>>>
    > {
  static auto parse(std::span<const uint8_t> data) ->
      typename TCommand::ParsingFormula::ValueType {
    static_assert(data.size() == TCommand::ParsingFormula::byteCount);
    typename TCommand::ParsingFormula::ValueType result = 0;
    for (auto byte : data) {
      result = result * 256 + byte;
    }
    return result / (TCommand::ParsingFormula::Nominator /
                     TCommand::ParsingFormula::Denominator);
  }
};

template <DiagnosticCommands::Command TCommand> struct Parser {
  static auto parse(std::span<const uint8_t>) ->
      typename TCommand::ParsingFormula::ValueType {
    return -1;
  }
};

#endif // PARSER_HPP
