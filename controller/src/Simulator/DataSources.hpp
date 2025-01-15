#ifndef DATA_SOURCES_HPP
#define DATA_SOURCES_HPP

#include "DiagnosticCommands.hpp"
#include <DataFrame.hpp>
#include <DataProvider.hpp>
#include <TimeProvider.hpp>
#include <concepts>
#include <vector>

template <typename TDataProviderManager>
concept IDataProviderManager = requires(TDataProviderManager dataProviderManager) {
  {
    dataProviderManager.template get<DiagnosticCommands::ENGINE_RPM>()
  } -> std::same_as<typename DiagnosticCommands::ENGINE_RPM::Encoding::ValueType>;
};

class RealDataProviderManager {
private:
  std::vector<DataFrame> data_;
  TimeProvider timeProvider_;

public:
  RealDataProviderManager(std::vector<DataFrame> data, TimeProvider timeProvider)
      : data_{data}, timeProvider_{timeProvider} {}

  template <DiagnosticCommands::Command TCommand> typename TCommand::Encoding::ValueType get() {
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::ENGINE_RPM>) {
      return data_[timeProvider_.get() % data_.size()].engineRPM;
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::VEHICLE_SPEED>) {
      return data_[timeProvider_.get() % data_.size()].vehicleSpeed;
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::ENGINE_LOAD>) {
      return data_[timeProvider_.get() % data_.size()].engineLoad;
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::THROTTLE_POSITION>) {
      return data_[timeProvider_.get() % data_.size()].throttlePosition;
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::UPTIME>) {
      return data_[timeProvider_.get() % data_.size()].uptime;
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::FUEL_LEVEL>) {
      return data_[timeProvider_.get() % data_.size()].fuelLevel;
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::ABSOLUTE_LOAD>) {
      return data_[timeProvider_.get() % data_.size()].absoluteLoad;
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::RELATIVE_THROTTLE_POSITION>) {
      return data_[timeProvider_.get() % data_.size()].relativeThrottlePosition;
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::ENGINE_FUEL_RATE>) {
      return data_[timeProvider_.get() % data_.size()].engineFuelRate;
    }
  };
};

static_assert(IDataProviderManager<RealDataProviderManager>);

class FakeDataProviderManager {

  DataProvider rpmProvider_{800, 100, 1, 800, 4000};
  DataProvider speedProvider_{10, 1, 1, 10, 140};
  DataProvider engineLoadProvider_{15, 2, 1, 0, 100};
  DataProvider throttlePositionProvider_{10, 5, 1, 0, 100};
  DataProvider uptimeProvider_{15, 1, 1, 5, 60};
  DataProvider fuelLevelProvider_{80, 2, -1, 5, 100};
  DataProvider absoluteLoadProvider_{10, 1, 1, 0, 20};
  DataProvider relativeThrottlePositionProvider_{20, 5, 1, 0, 90};
  DataProvider engineFuelRateProvider_{1200, 50, 1, 100, 2500};

public:
  template <DiagnosticCommands::Command TCommand> typename TCommand::Encoding::ValueType get() {
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::ENGINE_RPM>) {
      return rpmProvider_.get();
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::VEHICLE_SPEED>) {
      return speedProvider_.get();
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::ENGINE_LOAD>) {
      return engineLoadProvider_.get();
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::THROTTLE_POSITION>) {
      return throttlePositionProvider_.get();
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::UPTIME>) {
      return uptimeProvider_.get();
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::FUEL_LEVEL>) {
      return fuelLevelProvider_.get();
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::ABSOLUTE_LOAD>) {
      return absoluteLoadProvider_.get();
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::RELATIVE_THROTTLE_POSITION>) {
      return relativeThrottlePositionProvider_.get();
    }
    if constexpr (std::is_same_v<TCommand, DiagnosticCommands::ENGINE_FUEL_RATE>) {
      return engineFuelRateProvider_.get();
    }
  }
};

static_assert(IDataProviderManager<FakeDataProviderManager>);

#endif
