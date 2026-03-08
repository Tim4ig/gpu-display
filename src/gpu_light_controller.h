#pragma once

#include "types.h"

namespace tk {

class SapphireRX9060XTLedController {
 public:
  static constexpr auto kDefaultBus = 6;

  explicit SapphireRX9060XTLedController(i32 bus = kDefaultBus);

  SapphireRX9060XTLedController(const SapphireRX9060XTLedController&) = delete;
  auto operator=(const SapphireRX9060XTLedController&) -> SapphireRX9060XTLedController& = delete;

  SapphireRX9060XTLedController(SapphireRX9060XTLedController&&) = default;
  auto operator=(SapphireRX9060XTLedController&&) -> SapphireRX9060XTLedController& = default;

  ~SapphireRX9060XTLedController();

  void set(u32 rgba) const noexcept;

 private:
  [[nodiscard]] auto write_reg(u8 reg, u8 val) const -> bool;

  i32 bus_{};
  i32 fd_{};
};

} // namespace tk
