#pragma once

#include <esp_event.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include "logger.hpp"

class WiFi {
	static constexpr auto CONNECTED_BIT = BIT0;
	static Logger LOGGER;

public:
	static std::error_code initialize();
	static std::error_code connect(const std::string &ssid, const std::string &password, TickType_t timeout);

	static inline bool is_connected() {
		if (_is_initialized) {
			return (xEventGroupGetBitsFromISR(_event_group) & CONNECTED_BIT) != (EventBits_t)0;
		}
		return false;
	}

private:
	static EventGroupHandle_t _event_group;
	static bool _is_initialized;

	static void _on_got_ip(void *ctx, esp_event_base_t event_base, int32_t event_id, void *event_data);
	static void _event_handler(void *ctx, esp_event_base_t event_base, int32_t event_id, void *userdata);
};
