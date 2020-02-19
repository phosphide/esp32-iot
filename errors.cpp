#include "errors.hpp"

namespace { // anonymous namespace
  
struct ESPErrorCategory : std::error_category
{
  const char* name() const noexcept override;
  std::string message(int error_value) const override;
};
const char *ESPErrorCategory::name() const noexcept
{
  return "ESP";
}
std::string ESPErrorCategory::message(int32_t error_value) const
{
  return esp_err_to_name(error_value);
}
const ESPErrorCategory theESPErrorCategory {};


struct RuntimeErrorCategory : std::error_category
{
  const char* name() const noexcept override;
  std::string message(int error_value) const override;
};
const char *RuntimeErrorCategory::name() const noexcept
{
  return "Runtime";
}
std::string RuntimeErrorCategory::message(int32_t error_value) const
{
  switch (static_cast<RuntimeError>(error_value))
  {
  case RuntimeError::MQTTInitializationFailed:
      return "Initializing MQTT client failed";
  case RuntimeError::MQTTPublishFailed:
      return "Publishing MQTT data failed";
  case RuntimeError::MQTTSubscribeFailed:
      return "Subscribing to MQTT topic failed";
  default:
      return "Unknown error";
  }
}
const RuntimeErrorCategory theRuntimeErrorCategory {};
  
} // anonymous namespace
 
std::error_code make_error_code(ESPError e)
{
  return {static_cast<int32_t>(e), theESPErrorCategory};
}

std::error_code make_error_code(RuntimeError e)
{
  return {static_cast<int32_t>(e), theRuntimeErrorCategory};
}