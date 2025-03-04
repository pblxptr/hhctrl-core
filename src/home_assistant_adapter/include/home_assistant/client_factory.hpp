#pragma once

#include <boost/asio/io_context.hpp>
#include <home_assistant/adapter/broker_config.hpp>
#include <home_assistant/mqtt/async_mqtt_client.hpp>

namespace mgmt::home_assistant::adapter
{
    class MqttClientFactory
    {
    public:
        MqttClientFactory(boost::asio::io_context& ioc, BrokerConfig config)
            : ioc_{ioc}
            , config_{std::move(config)}
        {}

        [[nodiscard]] auto create_client(const std::string& unique_id) const
        {
            auto executor = ioc_.get_executor();

            return std::make_unique<mqtt::AsyncMqttClient<mqtt::ProtocolVersion_t::v3_1_1>>(
                std::move(executor),
                mqtt::ClientConfig {
                        .unique_id = unique_id,
                        .username = config_.username,
                        .password = config_.password,
                        .host = config_.host,
                        .port = config_.port,
                        .clean_session = true,
                        .keep_alive = config_.keep_alive_interval,
                        .max_attempts = config_.max_reconnect_attempts,
                        .reconnect_delay = config_.reconnect_delay
                }
            );
        }

    private:
        boost::asio::io_context& ioc_;
        BrokerConfig config_;
    };
}
