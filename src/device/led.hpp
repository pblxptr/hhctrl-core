#pragma once

#include <concepts>
#include <device/device.hpp>
#include <main_board/device/sysfs_led.hpp>

namespace mgmt::device {
template<class T>
concept Led = Device<T> and requires(T t)
{
  {
    t.set_brightness(std::declval<int>())
    } -> std::same_as<void>;
};

using Led_t = SysfsLed;
}// namespace mgmt::device