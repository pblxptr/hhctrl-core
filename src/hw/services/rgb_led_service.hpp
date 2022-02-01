#pragma once

#include "led_service.hpp"
#include <hw/drivers/led.hpp>

//TODO: Rename to indicator setrvice

namespace hw::services
{
  class RgbLedService : public LedService
  {
  public:
    RgbLedService(hw::drivers::LedDriver& rled, hw::drivers::LedDriver& gled, hw::drivers::LedDriver& bled);
    void set_state(common::data::IndicatorType, common::data::IndicatorState) const override;
    common::data::IndicatorState get_state(common::data::IndicatorType) const override;
  private:
    hw::drivers::LedDriver& red_;
    hw::drivers::LedDriver& green_;
    hw::drivers::LedDriver& blue_;
  };
}