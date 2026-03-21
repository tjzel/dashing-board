#ifndef SUZUKI_HPP
#define SUZUKI_HPP

#include <array>
#include <numbers>

namespace Suzuki {

// Suzuki SV650S drivetrain constants.
static constexpr double FINAL_GEAR_RATIO = 4.105;
static constexpr double REVERSE_GEAR_RATIO = -3.272; // Unused
static constexpr std::array<double, 5> GEAR_RATIOS = {3.454, 1.904, 1.28, 0.966, 0.757};

// Tire: 165/70 R14 — circumference in meters.
static constexpr double TIRE_CIRCUMFERENCE =
    2.0 * std::numbers::pi * ((14 * 0.0254 / 2.0) + (165 * 0.70 / 1000.0));

} // namespace Suzuki

#endif // SUZUKI_HPP
