#pragma once

#include <optional>

namespace hhctrl::hw {
  enum class HatchStatus {
    Open,
    Closed,
    ChangingPosition,
    Faulty,
    Undefined
  };
  class HatchDriver
  {
  public:
    virtual ~HatchDriver() = default;
    virtual void open() const = 0;
    virtual void close() const = 0;
    virtual HatchStatus status() const;
  };
}

