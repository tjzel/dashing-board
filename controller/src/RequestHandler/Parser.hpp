#ifndef PARSER_HPP
#define PARSER_HPP
#include <DiagnosticCommands.hpp>
#include <cassert>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <vector>

template <DiagnosticCommands::Command TCommand> struct DiagnosticCodec {
  static auto decode(std::span<const Byte> /*message*/) -> typename TCommand::Encoding::ValueType {
    throw std::runtime_error("No decoder found for command " + std::to_string(TCommand::mode) +
                             ":" + std::to_string(TCommand::pid));
  }

  static auto encode(const typename TCommand::Encoding::ValueType /*result*/) -> std::vector<Byte> {
    throw std::runtime_error("No encoder found for command " + std::to_string(TCommand::mode) +
                             ":" + std::to_string(TCommand::pid));
  }
};

// Test
static_assert(std::is_same_v<DiagnosticCommands::COMMAND_AVAILABILITY_00_1F::Encoding,
                             Encodings::CommandAvailability>);

template <DiagnosticCommands::Command TCommand>
  requires std::is_same_v<typename TCommand::Encoding, Encodings::CommandAvailability>
struct DiagnosticCodec<TCommand> {
  static auto decode(const std::span<const Byte> message) ->
      typename TCommand::Encoding::ValueType {
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

  static auto encode(const typename TCommand::Encoding::ValueType /*result*/) -> std::vector<Byte> {
    throw std::runtime_error("Not implemented");
  }
};

// Test
static_assert(
    std::is_same_v<DiagnosticCommands::VEHICLE_SPEED::Encoding, Encodings::Identity<1, int>>);

template <DiagnosticCommands::Command TCommand>
  requires std::is_same_v<
      typename TCommand::Encoding,
      Encodings::Identity<TCommand::Encoding::byteCount, typename TCommand::Encoding::ValueType>>
struct DiagnosticCodec<TCommand> {
  static auto decode(std::span<const Byte> message) ->

      typename TCommand::Encoding::ValueType {
    const auto data = message.subspan(2);
    assert(data.size() == TCommand::Encoding::byteCount);
    typename TCommand::Encoding::ValueType result = 0;
    for (auto byte : data) {
      result = result * 256 + byte;
    }
    return result;
  }

  static auto encode(typename TCommand::Encoding::ValueType result) -> std::vector<Byte> {
    std::vector<Byte> data(TCommand::Encoding::byteCount);
    for (size_t i = 0; i < data.size(); i++) {
      data[i] = result % 256;
      result /= 256;
    }
    return data;
  }
};

// Test
static_assert(
    std::is_same_v<DiagnosticCommands::ENGINE_RPM::Encoding, Encodings::MultiplyBy<2, int, 1, 4>>);
static_assert(!std::is_same_v<
              DiagnosticCommands::ABSOLUTE_LOAD::Encoding,
              Encodings::MultiplyBy<DiagnosticCommands::ABSOLUTE_LOAD::Encoding::byteCount,
                                    DiagnosticCommands::ABSOLUTE_LOAD::Encoding::ValueType,
                                    DiagnosticCommands::ABSOLUTE_LOAD::Encoding::Nominator,
                                    DiagnosticCommands::ABSOLUTE_LOAD::Encoding::Denominator>>);
static_assert(
    !std::is_same_v<DiagnosticCommands::FUEL_LEVEL::Encoding,
                    Encodings::MultiplyBy<DiagnosticCommands::FUEL_LEVEL::Encoding::byteCount,
                                          DiagnosticCommands::FUEL_LEVEL::Encoding::ValueType,
                                          DiagnosticCommands::FUEL_LEVEL::Encoding::Nominator,
                                          DiagnosticCommands::FUEL_LEVEL::Encoding::Denominator>>);
static_assert(std::is_base_of_v<
              Encodings::MultiplyBy<DiagnosticCommands::ABSOLUTE_LOAD::Encoding::byteCount,
                                    DiagnosticCommands::ABSOLUTE_LOAD::Encoding::ValueType,
                                    DiagnosticCommands::ABSOLUTE_LOAD::Encoding::Nominator,
                                    DiagnosticCommands::ABSOLUTE_LOAD::Encoding::Denominator>,
              DiagnosticCommands::ABSOLUTE_LOAD::Encoding>);
static_assert(
    std::is_base_of_v<Encodings::MultiplyBy<DiagnosticCommands::FUEL_LEVEL::Encoding::byteCount,
                                            DiagnosticCommands::FUEL_LEVEL::Encoding::ValueType,
                                            DiagnosticCommands::FUEL_LEVEL::Encoding::Nominator,
                                            DiagnosticCommands::FUEL_LEVEL::Encoding::Denominator>,
                      DiagnosticCommands::FUEL_LEVEL::Encoding>);

template <DiagnosticCommands::Command TCommand>
  requires std::is_same_v<typename TCommand::Encoding,
                          Encodings::MultiplyBy<
                              TCommand::Encoding::byteCount, typename TCommand::Encoding::ValueType,
                              TCommand::Encoding::Nominator, TCommand::Encoding::Denominator>> ||
           std::is_base_of_v<
               Encodings::MultiplyBy<
                   TCommand::Encoding::byteCount, typename TCommand::Encoding::ValueType,
                   TCommand::Encoding::Nominator, TCommand::Encoding::Denominator>,
               typename TCommand::Encoding>
struct DiagnosticCodec<TCommand> {
  static auto decode(const std::span<const Byte> message) ->
      typename TCommand::Encoding::ValueType {
    const auto data = message.subspan(2);
    assert(data.size() == TCommand::Encoding::byteCount);
    typename TCommand::Encoding::ValueType result = 0;
    for (auto byte : data) {
      result = result * 256 + byte;
    }
    result = result * TCommand::Encoding::Nominator / TCommand::Encoding::Denominator;
    return result;
  }

  static auto encode(typename TCommand::Encoding::ValueType result) -> std::vector<Byte> {
    result = result * TCommand::Encoding::Denominator / TCommand::Encoding::Nominator;

    std::vector<Byte> data(TCommand::Encoding::byteCount);
    for (int i = data.size() - 1; i > -1; i--) {
      data[i] = (int)result % 256;
      result /= 256;
    }

    return data;
  }
};

#endif // PARSER_HPP
