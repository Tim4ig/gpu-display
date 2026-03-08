#include "gpu_light_controller.h"

#include "types.h"

#include <array>
#include <fcntl.h>
#include <format>
#include <linux/i2c-dev.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

namespace {
constexpr tk::u8 kSapphireAddr = 0x28;

constexpr tk::u8 kRegExternalControl = 0x0F;
constexpr tk::u8 kRegMode = 0x10;
constexpr tk::u8 kRegRed = 0x1A;
constexpr tk::u8 kRegGreen = 0x1B;
constexpr tk::u8 kRegBlue = 0x1C;

constexpr tk::u8 kModeCustom = 0x06;
constexpr tk::u8 kModeOff = 0x07;

constexpr auto kShiftRed = 24U;
constexpr auto kShiftGreen = 16U;
constexpr auto kShiftBlue = 8U;

// Hardware channel range: 0–26 (verified empirically).
// Values above 26 wrap back to 0, so we must scale down from the 0–255 sw range.
constexpr auto kSwChannelMax = 255U;
constexpr auto kHwChannelMax = 26U;

} // namespace

namespace tk {

SapphireRX9060XTLedController::SapphireRX9060XTLedController(const i32 bus)
  : bus_(bus) {
  const auto path = std::format("/dev/i2c-{}", bus_);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
  fd_ = open(path.c_str(), O_RDWR);

  if (fd_ < 0) {
    throw std::runtime_error(std::format("Failed to open I2C bus {}", bus_));
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
  if (ioctl(fd_, I2C_SLAVE, kSapphireAddr) < 0) {
    close(fd_);
    throw std::runtime_error(std::format("Failed to set I2C slave address on bus {}", bus_));
  }
}

SapphireRX9060XTLedController::~SapphireRX9060XTLedController() {
  if (fd_ >= 0) {
    set(0);
    close(fd_);
  }
}

void SapphireRX9060XTLedController::set(const u32 rgba) const noexcept {
  if (!write_reg(kRegExternalControl, 0x00)) {
    return;
  }

  const auto red_sw = rgba >> kShiftRed & kSwChannelMax;
  const auto green_sw = rgba >> kShiftGreen & kSwChannelMax;
  const auto blue_sw = rgba >> kShiftBlue & kSwChannelMax;
  const auto alpha = rgba & kSwChannelMax;

  if (rgba == 0U || alpha == 0U) {
    if (!write_reg(kRegMode, kModeOff)) {
    }
    return;
  }

  // Scale 0–255 sw value with alpha brightness to 0–26 hw range.
  // hw = sw * alpha * 26 / (255 * 255). Max intermediate: 255*255*26 < 2^21, fits u32.
  const auto scale = [](const u32 sw, const u32 alpha_ch) -> u8 {
    return static_cast<u8>(sw * alpha_ch * kHwChannelMax / (kSwChannelMax * kSwChannelMax));
  };

  if (!write_reg(kRegMode, kModeCustom)) {
    return;
  }
  if (!write_reg(kRegRed, scale(red_sw, alpha))) {
    return;
  }
  if (!write_reg(kRegGreen, scale(green_sw, alpha))) {
    return;
  }
  if (!write_reg(kRegBlue, scale(blue_sw, alpha))) {
  }
}

auto SapphireRX9060XTLedController::write_reg(const u8 reg, const u8 val) const -> bool {
  const std::array<u8, 2> buf = {reg, val};
  return write(fd_, buf.data(), buf.size()) == static_cast<ssize_t>(buf.size());
}

} // namespace tk
