#pragma once

#include "types.h"

namespace tk {

// Exponential moving average filter.
// Each new sample contributes kAlpha weight; older samples decay geometrically.
// This avoids the sliding-window exit artifact of a simple moving average.
class TempFilter final {
 public:
  // Low alpha = large effective window (~39 samples at 100 ms = ~4 s).
  static constexpr auto kAlpha = 0.05F;

  void push(f32 sample);
  [[nodiscard]] auto value() const -> f32;

 private:
  f32  value_{};
  bool initialized_{false};
};

} // namespace tk
