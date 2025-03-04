//
// Created by bielpa on 24.08.23.
//

#pragma once

#include <system_error>
#include <boost/system/errc.hpp>
#include <fmt/format.h>

namespace mgmt::home_assistant::mqtt
{
  enum class ErrorCode {
    Timeout = 1 ,
    NoService,
    ConnectionRefused,
    UnknownPacket,
    QosNotSupported,
    Disconnected,
    Reconnected,
    InvalidConfig,
    PublishFailure,
    SubscriptionFailure,
  };

  namespace detail {
    struct ErrorCategory : std::error_category
    {
      const char* name() const noexcept override
      {
        return "mqtt_home_assistant";
      }

      std::string message(int error) const override
      {
        switch (static_cast<ErrorCode>(error)) {
          case ErrorCode::Timeout:
            return "timeout";
          case ErrorCode::NoService:
            return "no_service";
        case ErrorCode::ConnectionRefused:
            return "connection_refused";
          case ErrorCode::UnknownPacket:
            return "unknown_packet";
          case ErrorCode::QosNotSupported:
            return "qos_not_supported";
          case ErrorCode::Reconnected:
            return "reconnected";
          case ErrorCode::Disconnected:
            return "disconnected";
          case ErrorCode::InvalidConfig:
            return "invalid_config";
          case ErrorCode::PublishFailure:
            return "publish_failure";
          case ErrorCode::SubscriptionFailure:
            return "subscription_failure";
          default:
            return "unrecognized_error";
        }
      }
    };

    inline ErrorCode map_error_code(boost::system::error_code error_code)
    {
      switch (error_code.value()) {
      case boost::system::errc::timed_out:
        return ErrorCode::Timeout;
      case boost::system::errc::network_unreachable:
      case boost::system::errc::no_such_file_or_directory:
        return ErrorCode::NoService;
      case boost::system::errc::connection_refused:
          return ErrorCode::ConnectionRefused;
      default:
        throw std::runtime_error{ fmt::format("Error does not have appropriate mapping: {}, errc: {}", error_code.what(), error_code.value())};
      }
    }
  } // namespace detail

  std::error_code make_error_code(ErrorCode error);

} // namespace mgmt::home_assistant::mqtt


namespace std
{
  template <>
  struct is_error_code_enum<mgmt::home_assistant::mqtt::ErrorCode> : true_type {};
} // namespace std


