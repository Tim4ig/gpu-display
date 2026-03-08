#pragma once

#include "types.h"

#include <string>

namespace tk {

class CpuTempSensor final {
 public:
  explicit CpuTempSensor(std::string path = "/sys/class/hwmon/hwmon2/temp1_input");

  [[nodiscard]] auto scan() const -> f32;

 private:
  std::string path_;
};

} // namespace tk
