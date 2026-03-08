#include "cpu_temp_sensor.h"

#include "types.h"

#include <fstream>
#include <limits>
#include <string>
#include <utility>

namespace {
constexpr auto kMilliDegreeToC = 1000.0F;
} // namespace

namespace tk {

CpuTempSensor::CpuTempSensor(std::string path)
  : path_(std::move(path)) {
}

auto CpuTempSensor::scan() const -> f32 {
  std::ifstream file{path_};
  if (!file) {
    return std::numeric_limits<f32>::quiet_NaN();
  }

  int raw_temp{};
  if (!(file >> raw_temp)) {
    return std::numeric_limits<f32>::quiet_NaN();
  }

  return static_cast<f32>(raw_temp) / kMilliDegreeToC;
}

} // namespace tk
