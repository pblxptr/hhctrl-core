#include "board_ctrl_server.hpp"

#include <icon/endpoint/endpoint_config.hpp>

#include <common/utils/enum_mapper.hpp>
#include <common/data/indicator.hpp>
#include <common/mapper/indicator_mapper_config.hpp>

namespace {
}

using namespace boost::asio;

namespace hw::board_ctrl
{
BoardControlServer::BoardControlServer(
  hw::services::LedService& led_service,
  boost::asio::io_context& bctx,
  zmq::context_t& zctx,
  std::string address
)
  : led_service_{led_service}
{
  endpoint_ = icon::setup_default_endpoint(
    icon::use_services(bctx, zctx),
    icon::address(address),
    icon::consumer<bci::GetVisualIndicationReq>([this](auto& context) -> awaitable<void> { co_await handle(context); }),
    icon::consumer<bci::SetVisualIndicationReq>([this](auto& context) -> awaitable<void> { co_await handle(context); }),
    icon::consumer<bci::RestartBoardFwd>([this](auto& context) -> awaitable<void>  { co_await handle(context); }))
  .build();
}

awaitable<void> BoardControlServer::run()
{
  co_return;
}


awaitable<void> BoardControlServer::handle(icon::MessageContext<bci::GetVisualIndicationReq>& context)
{
  spdlog::debug("BoardControlServer: handle GetVisualIndicationReq");

  auto& message = context.message();
  const auto indicator_type = common::mapper::IndicatorTypeMapper_t::map_safe<common::data::IndicatorType>(message.indicator());
  const auto indicator_state = led_service_.get_state(indicator_type);

  auto response = bci::GetVisualIndicationCfm{};
  response.set_indicator(message.indicator());
  response.set_state(common::mapper::IndicatorStateMapper_t::map_safe<bci::IndicatorState>(indicator_state));

  co_await context.async_respond(std::move(response));
}

awaitable<void> BoardControlServer::handle(icon::MessageContext<bci::SetVisualIndicationReq>& context)
{
  spdlog::debug("BoardControlServer: handle SetVisualIndicationReq");

  auto& message = context.message();
  const auto indicator_type = common::mapper::IndicatorTypeMapper_t::map_safe<common::data::IndicatorType>(message.indicator());
  const auto indicator_state = common::mapper::IndicatorStateMapper_t::map_safe<common::data::IndicatorState>(message.state());

  led_service_.set_state(indicator_type, indicator_state);

  co_await context.async_respond(bci::SetVisualIndicationCfm{});
}

awaitable<void> BoardControlServer::handle(icon::MessageContext<bci::RestartBoardFwd>& context)
{
  spdlog::debug("BoardControlServer: handle RestartBoardFwd. Not implemented");

  co_return;
}

}