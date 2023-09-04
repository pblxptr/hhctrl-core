//
// Created by pp on 7/24/22.
//

#include <algorithm>

#include <home_assistant/adapter/hatch_event_handler.hpp>
#include <home_assistant/adapter/logger.hpp>
#include <coro/co_spawn.hpp>

namespace mgmt::home_assistant::adapter {
HatchEventHandler::HatchEventHandler(
  const EntityFactory& factory,
  const DeviceIdentityProvider& device_identity_provider)
  : factory_{ factory }
  , device_identity_provider_{ device_identity_provider }
{
  common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("HatchEventHandler::{}", __FUNCTION__);
}

boost::asio::awaitable<void> HatchEventHandler::operator()([[maybe_unused]] const DeviceCreated_t& event)
{
  common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("HatchEventHandler::{}(DeviceCreated)", __FUNCTION__);

  auto hatch_handler = co_await HatchHandler::async_create(event.device_id, device_identity_provider_, factory_);
  if (!hatch_handler) {
      common::logger::get(mgmt::home_assistant::adapter::Logger)->error("HatchEventHandler, cannot create HatchHandler");

      co_return;
  }

  hatches_.push_back(std::move(*hatch_handler));

  auto executor = co_await boost::asio::this_coro::executor;

  boost::asio::co_spawn(executor, hatches_.back().async_run(), common::coro::rethrow);
}

boost::asio::awaitable<void> HatchEventHandler::operator()(const DeviceRemoved_t& event)
{
  common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("HatchEventHandler::{}(DeviceRemoved)", __FUNCTION__);

  const auto device_id = event.device_id;
  const auto erased = std::erase_if(hatches_, [device_id](auto& hatch) {
    return device_id == hatch.hardware_id();
  });

  if (erased == 0) {
    throw std::runtime_error(fmt::format("Hatch device with id: {} was not found", device_id));
  }

  co_return;
}

boost::asio::awaitable<void> HatchEventHandler::operator()([[maybe_unused]] const DeviceStateChanged_t& event)
{
  common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("HatchEventHandler::{}(DeviceStateChanged)", __FUNCTION__);

  const auto device_id = event.device_id;
  auto hatch = std::ranges::find_if(hatches_, [device_id](auto& hatch) {
    return device_id == hatch.hardware_id();
  });

  if (hatch == hatches_.end()) {
    throw std::runtime_error(fmt::format("Hatch device with id: {} was not found", device_id));
  }

  co_await hatch->async_sync_state();
}
}// namespace mgmt::home_assistant::device
