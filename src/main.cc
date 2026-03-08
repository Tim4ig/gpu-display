#include "cpu_temp_sensor.h"
#include "gpu_light_controller.h"
#include "temp_filter.h"
#include "types.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <exception>
#include <thread>

namespace {

constexpr auto kHueSector = 60.0F;
constexpr auto kHue120 = 120.0F;
constexpr auto kHue180 = 180.0F;
constexpr auto kHueMax = 240.0F;
constexpr auto kHue300 = 300.0F;
constexpr auto kHueMod = 2.0F;
constexpr auto kChannelMax = 255.0F;
constexpr auto kAlphaFull = 80U; // limit alpha
constexpr auto kShiftRed = 24U;
constexpr auto kShiftGreen = 16U;
constexpr auto kShiftBlue = 8U;
constexpr auto kDisplayIntervalMs = 33U; // ~30 fps
constexpr auto kTempMin = 50.0F;
constexpr auto kTempMax = 95.0F;
constexpr auto kTempRange = kTempMax - kTempMin;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::atomic running{true};

void signal_handler([[maybe_unused]] int signum) {
  running = false;
}

tk::u32 cpu_temp_to_rgba(const tk::f32 celsius) {
  const auto clamped = std::clamp((celsius - kTempMin) / kTempRange, 0.0F, 1.0F);
  const auto hue = (1.0F - clamped) * kHueMax;
  const auto second = 1.0F - std::fabs(std::fmod(hue / kHueSector, kHueMod) - 1.0F);

  auto red_f = 0.0F;
  auto green_f = 0.0F;
  auto blue_f = 0.0F;

  if (hue < kHueSector) {
    red_f = 1.0F;
    green_f = second;
  } else if (hue < kHue120) {
    red_f = second;
    green_f = 1.0F;
  } else if (hue < kHue180) {
    green_f = 1.0F;
    blue_f = second;
  } else if (hue < kHueMax) {
    green_f = second;
    blue_f = 1.0F;
  } else if (hue < kHue300) {
    red_f = second;
    blue_f = 1.0F;
  } else {
    red_f = 1.0F;
    blue_f = second;
  }

  const auto rr = static_cast<tk::u32>(red_f * kChannelMax);
  const auto gg = static_cast<tk::u32>(green_f * kChannelMax);
  const auto bb = static_cast<tk::u32>(blue_f * kChannelMax);

  return rr << kShiftRed | gg << kShiftGreen | bb << kShiftBlue | kAlphaFull;
}

} // namespace

int main() try {
  const tk::SapphireRX9060XTLedController controller;
  const tk::CpuTempSensor sensor;
  tk::TempFilter filter;

  signal(SIGINT, signal_handler);

  const auto initial = [&]() -> tk::f32 {
    const auto temp = sensor.scan();
    return std::isfinite(temp) ? temp : kTempMin;
  }();
  filter.push(initial);

  while (running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(kDisplayIntervalMs));

    filter.push(sensor.scan());

    const auto temp = filter.value();
    if (std::isfinite(temp)) {
      controller.set(cpu_temp_to_rgba(temp));
    }
  }

  controller.set(0);

  return 0;
} catch (const std::exception& ex) {
  std::fputs("Fatal: ", stderr);
  std::fputs(ex.what(), stderr);
  std::fputs("\n", stderr);
  return 1;
}
