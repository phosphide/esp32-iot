#pragma once

#include <iostream>
#include <system_error>

#include <esp_err.h>
#include <esp_system.h>

#include "logger.hpp"

#define RETURN_IF_ERROR(function_result)                                                                               \
	do {                                                                                                                 \
		if (function_result != ESP_OK) {                                                                                   \
			std::error_code ec = to_strong_error(function_result);                                                           \
			return ec;                                                                                                       \
		}                                                                                                                  \
	} while (0)

inline void RESTART_IF_ERROR(const std::error_code &ec) {
	if (ec) {
		Logger::log_globally("[CRITICAL]: {} - RESTARTING", ec.message());
		esp_restart();
	}
}

inline void RESTART_IF_ERROR(esp_err_t esp_error) {
	if (esp_error != ESP_OK) {
		Logger::log_globally("[CRITICAL]: {} - RESTARTING", esp_err_to_name(esp_error));
		esp_restart();
	}
}

enum class ESPError : esp_err_t {};

enum class RuntimeError {
	MQTTInitializationFailed = 1,
	MQTTPublishFailed = 2,
	MQTTSubscribeFailed = 3,
};

inline ESPError to_strong_error(esp_err_t err) {
	return static_cast<ESPError>(err);
}

namespace std {
template <> struct is_error_code_enum<ESPError> : true_type {};

template <> struct is_error_code_enum<RuntimeError> : true_type {};
} // namespace std

std::error_code make_error_code(ESPError);
std::error_code make_error_code(RuntimeError);
