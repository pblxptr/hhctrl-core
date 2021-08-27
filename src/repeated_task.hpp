#pragma once

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

#include "task.hpp"
#include <spdlog/spdlog.h>

namespace hhctrl::core::scheduler
{
class RepeatedTask : public Task
{
public:
  template<class THandler>
  RepeatedTask(
    boost::asio::io_context& io,
    std::string name,
    Timepoint_t expiry,
    std::chrono::days interval,
    THandler&& handler
  )
    : name_{std::move(name)}
    , timer_{io, std::move(expiry)}
    , repeat_interval_(std::move(interval))
    , handler_{std::forward<THandler>(handler)}
  {}

  const std::string& name() const override
  {
    return name_;
  }

  void set_expiry(const Timepoint_t& expiry) override
  {
    timer_.expires_at(expiry);
  }

  void install() override
  {
    spdlog::info(fmt::format("RepeatedTask::install() expiry: {}", utils::datetime::to_string(timer_.expiry())));
    timer_.async_wait([this](const auto& ec)
    {
      if (ec) {
        return;
      }
      handler_();
    });
  }

  void reconfigure() override
  {
    timer_.expires_at(timer_.expiry() + repeat_interval_);
  }

  bool is_reinstallable() const override
  {
    return true;
  }

  Timepoint_t expiry() const override
  {
    return timer_.expiry();
  }

  std::string to_string() const override
  {
    return utils::datetime::to_string(timer_.expiry());
  }

private:
  Id_t id_;
  std::string name_;
  std::chrono::days repeat_interval_;
  TaskHandler_t handler_;
  boost::asio::system_timer timer_;
};
}