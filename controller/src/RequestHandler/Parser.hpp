#ifndef PARSER_HPP
#define PARSER_HPP
#include <DiagnosticCommands.hpp>
#include <cassert>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <vector>

template <DiagnosticCommands::Command TCommand> struct Parser {
  static auto parse(std::span<const Byte> /*message*/) -> typename TCommand::ParsingFormula::ValueType {
    throw std::runtime_error("No parser found for command " + std::to_string(TCommand::mode) + ":" +
                             std::to_string(TCommand::pid));
  }

  static auto reverseParse(const typename TCommand::ParsingFormula::ValueType /*result*/) -> std::vector<Byte> {
    throw std::runtime_error("No reverse parser found for command " + std::to_string(TCommand::mode) + ":" +
                             std::to_string(TCommand::pid));
  }
};

// Test
static_assert(
    std::is_same_v<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::ParsingFormula, ParsingFormulas::CommandAvailability>);

template <DiagnosticCommands::Command TCommand>
  requires std::is_same_v<typename TCommand::ParsingFormula, ParsingFormulas::CommandAvailability>
struct Parser<TCommand> {
  static auto parse(const std::span<const Byte> message) -> typename TCommand::ParsingFormula::ValueType {
    const auto data = message.subspan(2);
    std::map<CommandLiteral, bool> availability;
    const auto commandOffset = TCommand::pid + 1;
    auto byteOffset = 0;
    for (const auto readByte : data) {
      for (int i = 0; i < 8; i++) {
        const bool bit = (readByte & (1 << (7 - i))) != 0;
        if (!bit) {
          continue;
        }
        const Byte keyOffset = commandOffset + byteOffset + i;
        const CommandLiteral key = {TCommand::mode, keyOffset};
        availability[key] = bit;
      }
      byteOffset += 8;
    }
    return availability;
  }

  static auto reverseParse(const typename TCommand::ParsingFormula::ValueType /*result*/) -> std::vector<Byte> {
    throw std::runtime_error("Not implemented");
  }
};

// Test
static_assert(std::is_same_v<DiagnosticCommands::VEHICLE_SPEED::ParsingFormula, ParsingFormulas::Identity<1, int>>);

template <DiagnosticCommands::Command TCommand>
  requires std::is_same_v<
      typename TCommand::ParsingFormula,
      ParsingFormulas::Identity<TCommand::ParsingFormula::byteCount, typename TCommand::ParsingFormula::ValueType>>
struct Parser<TCommand> {
  static auto parse(std::span<const Byte> message) ->

      typename TCommand::ParsingFormula::ValueType {
    const auto data = message.subspan(2);
    assert(data.size() == TCommand::ParsingFormula::byteCount);
    typename TCommand::ParsingFormula::ValueType result = 0;
    for (auto byte : data) {
      result = result * 256 + byte;
    }
    return result;
  }

  static auto reverseParse(typename TCommand::ParsingFormula::ValueType result) -> std::vector<Byte> {
    std::vector<Byte> data(TCommand::ParsingFormula::byteCount);
    for (size_t i = 0; i < data.size(); i++) {
      data[i] = result % 256;
      result /= 256;
    }
    return data;
  }
};

// Test
static_assert(std::is_same_v<DiagnosticCommands::ENGINE_RPM::ParsingFormula, ParsingFormulas::MultiplyBy<2, int, 1, 4>>);
static_assert(!std::is_same_v<DiagnosticCommands::ABSOLUTE_LOAD::ParsingFormula,
                              ParsingFormulas::MultiplyBy<DiagnosticCommands::ABSOLUTE_LOAD::ParsingFormula::byteCount,
                                                          DiagnosticCommands::ABSOLUTE_LOAD::ParsingFormula::ValueType,
                                                          DiagnosticCommands::ABSOLUTE_LOAD::ParsingFormula::Nominator,
                                                          DiagnosticCommands::ABSOLUTE_LOAD::ParsingFormula::Denominator>>);
static_assert(!std::is_same_v<DiagnosticCommands::FUEL_LEVEL::ParsingFormula,
                              ParsingFormulas::MultiplyBy<DiagnosticCommands::FUEL_LEVEL::ParsingFormula::byteCount,
                                                          DiagnosticCommands::FUEL_LEVEL::ParsingFormula::ValueType,
                                                          DiagnosticCommands::FUEL_LEVEL::ParsingFormula::Nominator,
                                                          DiagnosticCommands::FUEL_LEVEL::ParsingFormula::Denominator>>);
static_assert(std::is_base_of_v<ParsingFormulas::MultiplyBy<DiagnosticCommands::ABSOLUTE_LOAD::ParsingFormula::byteCount,
                                                            DiagnosticCommands::ABSOLUTE_LOAD::ParsingFormula::ValueType,
                                                            DiagnosticCommands::ABSOLUTE_LOAD::ParsingFormula::Nominator,
                                                            DiagnosticCommands::ABSOLUTE_LOAD::ParsingFormula::Denominator>,
                                DiagnosticCommands::ABSOLUTE_LOAD::ParsingFormula>);
static_assert(std::is_base_of_v<ParsingFormulas::MultiplyBy<DiagnosticCommands::FUEL_LEVEL::ParsingFormula::byteCount,
                                                            DiagnosticCommands::FUEL_LEVEL::ParsingFormula::ValueType,
                                                            DiagnosticCommands::FUEL_LEVEL::ParsingFormula::Nominator,
                                                            DiagnosticCommands::FUEL_LEVEL::ParsingFormula::Denominator>,
                                DiagnosticCommands::FUEL_LEVEL::ParsingFormula>);

template <DiagnosticCommands::Command TCommand>
  requires std::is_same_v<
               typename TCommand::ParsingFormula,
               ParsingFormulas::MultiplyBy<TCommand::ParsingFormula::byteCount, typename TCommand::ParsingFormula::ValueType,
                                           TCommand::ParsingFormula::Nominator, TCommand::ParsingFormula::Denominator>> ||
           std::is_base_of_v<
               ParsingFormulas::MultiplyBy<TCommand::ParsingFormula::byteCount, typename TCommand::ParsingFormula::ValueType,
                                           TCommand::ParsingFormula::Nominator, TCommand::ParsingFormula::Denominator>,
               typename TCommand::ParsingFormula>
struct Parser<TCommand> {
  static auto parse(const std::span<const Byte> message) -> typename TCommand::ParsingFormula::ValueType {
    const auto data = message.subspan(2);
    assert(data.size() == TCommand::ParsingFormula::byteCount);
    typename TCommand::ParsingFormula::ValueType result = 0;
    for (auto byte : data) {
      result = result * 256 + byte;
    }
    result = result * TCommand::ParsingFormula::Nominator / TCommand::ParsingFormula::Denominator;
    return result;
  }

  static auto reverseParse(typename TCommand::ParsingFormula::ValueType result) -> std::vector<Byte> {
    result = result * TCommand::ParsingFormula::Denominator / TCommand::ParsingFormula::Nominator;

    std::vector<Byte> data(TCommand::ParsingFormula::byteCount);
    for (int i = data.size() - 1; i > -1; i--) {
      data[i] = result % 256;
      result /= 256;
    }

    return data;
  }
};

#endif // PARSER_HPP
