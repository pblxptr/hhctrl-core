#pragma once
#include <boost/functional/hash.hpp>

#include <poller/polling_service.hpp>
#include <poller/poller_factory.hpp>
#include <event/event_bus.hpp>
#include <device/devicetree.hpp>
#include <device/hardware_identity_store.hpp>
#include <main_board/platform/platform_builder.hpp>
#include <main_board/platform/platform_device_discovery.hpp>
#include <main_board/platform/generic_device_loading_strategy.hpp>

#include <main_board/platform/sysfs_rgb_indicator_provider.hpp>
#include <main_board/platform/hatch2sr_provider.hpp>
#include <main_board/platform/sysfs_ds18b20_provider.hpp>
#include <main_board/platform/sysfs_cpu_temp_sensor_provider.hpp>

#include <poller/main_board_poller.hpp>
#include <utility>

namespace mgmt::app {
inline void main_board_init(
  std::string pdtree_path,
  mgmt::device::DeviceTree& dtree,
  mgmt::device::HardwareIdentityStore_t& hw_identity_store,
  mgmt::poller::PollingService& polling_service,
  common::event::AsyncEventBus& bus)
{
  using namespace mgmt::platform_device;// NOLINT(google-build-using-namespace)

  // Prepare
  auto poller_factory = mgmt::poller::PollerFactory{ bus };
  auto builder = PlatformBuilder<DefaultGenericDeviceLoadingStrategy>{};
  auto platform_device_discovery = PlatformDeviceDiscovery{
    std::move(pdtree_path),
    RGBIndicatorProvider{},
    HatchProvider{ polling_service, poller_factory },
    SysfsDS18B20Provider{ polling_service, poller_factory },
    SysfsCPUTempSensorProvider{ polling_service, poller_factory }
  };
  platform_device_discovery.setup(builder);

  // Handle main board
  auto board_id = mgmt::device::register_device(std::move(builder).build_board());
  const mgmt::device::Device auto& board = mgmt::device::get_device<mgmt::device::MainBoard>(board_id);
  hw_identity_store.emplace(board_id, board.hardware_identity());
  polling_service.add_poller(board_id,
    std::chrono::seconds(5),
    poller_factory.create_poller<mgmt::poller::MainBoardPoller>(board_id));
  bus.publish(mgmt::event::DeviceCreated<mgmt::device::MainBoard>{
    board_id });

  // Handle generic devices connected to board
  auto generic_dev_loader_handler = DefaultGenericDeviceLoadingStrategy{
    board_id,
    dtree,
    bus
  };
  auto generic_dev_loaders = std::move(builder).build_generic_loaders();// NOLINT(hicpp-invalid-access-moved,bugprone-use-after-move)
  for (auto&& loader : generic_dev_loaders) {// call ref&& qualified method
    loader(generic_dev_loader_handler);
  }
}
}// namespace mgmt::app
