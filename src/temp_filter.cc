#include "temp_filter.h"

#include "types.h"

#include <cmath>
#include <limits>

namespace tk {

void TempFilter::push(const f32 sample) {
  if (!std::isfinite(sample)) {
    return;
  }
  if (!initialized_) {
    value_ = sample;
    initialized_ = true;
  } else {
    value_ = (value_ * (1.0F - kAlpha)) + (sample * kAlpha);
  }
}

auto TempFilter::value() const -> f32 {
  if (!initialized_) {
    return std::numeric_limits<f32>::quiet_NaN();
  }
  return value_;
}

} // namespace tk
