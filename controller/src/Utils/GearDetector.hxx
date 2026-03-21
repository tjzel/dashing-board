#ifndef GEAR_DETECTOR_HPP
#define GEAR_DETECTOR_HPP

#include <cmath>
#include <vector>

class GearDetector {
public:
  GearDetector(double finalGearRatio, std::vector<double> gearRatios, double tireCircumference)
      : finalGearRatio_(finalGearRatio), gearRatios_(std::move(gearRatios)),
        tireCircumference_(tireCircumference) {}

  // Returns the detected gear (1-indexed), or 0 if neutral/unknown.
  [[nodiscard]] int detect(int rpm, int speedKmh) const {
    if (rpm <= 0 || speedKmh <= 0)
      return 0;

    double kphToRpm = 1000.0 / 60.0 / tireCircumference_;
    double measuredGearRatio = rpm / (speedKmh * kphToRpm * finalGearRatio_);

    int bestGear = 0;
    double bestRelError = 1e9;
    for (size_t i = 0; i < gearRatios_.size(); i++) {
      double relError = std::abs(gearRatios_[i] - measuredGearRatio) / gearRatios_[i];
      if (relError < bestRelError) {
        bestRelError = relError;
        bestGear = static_cast<int>(i) + 1;
      }
    }

    // Reject if best match is more than 15% off.
    if (bestRelError > 0.15)
      return 0;

    return bestGear;
  }

private:
  double finalGearRatio_;
  std::vector<double> gearRatios_;
  double tireCircumference_;
};

#endif // GEAR_DETECTOR_HPP
