#pragma once

#include <iostream>
#include <system_error>

#include "esp_err.h"


#define RETURN_IF_ERROR(function_result) \
do { \
  if (function_result != ESP_OK) { \
    std::error_code ec = to_strong_error(function_result); \
    return ec; \
  } \
} while (0)
#define RESTART_IF_ERROR(ec) \
do { \
  if (ec) { \
    std::cerr << "CRITICAL: " << ec.message() << " - RESTARTING" << std::endl; \
    esp_restart(); \
  } \
} while (0)

enum class ESPError : esp_err_t {};

enum class RuntimeError {
  MQTTInitializationFailed = 1,
  MQTTPublishFailed = 2,
};

inline ESPError to_strong_error(esp_err_t err) {
    return static_cast<ESPError>(err);
}

namespace std
{
  template <>
  struct is_error_code_enum<ESPError> : true_type {};

  template <>
  struct is_error_code_enum<RuntimeError> : true_type {};
}
 
std::error_code make_error_code(ESPError);
std::error_code make_error_code(RuntimeError);
