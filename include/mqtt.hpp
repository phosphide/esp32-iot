#pragma once

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <functional>
#include <map>
#include <mqtt_client.h>
#include <vector>

#include "logger.hpp"

class MQTTClient {
	static Logger LOGGER;
	static constexpr auto CONNECTED_BIT = BIT0;

public:
	static std::error_code initialize(const std::string &client_id, const std::string &broker_address,
	                                  const std::string &last_will_topic, TickType_t timeout);
	static std::error_code publish(const std::string &topic, const std::string &data, bool retain = false);
	static std::error_code subscribe(const std::string &topic);
	static void add_connection_callback(std::function<void()> callback);
	static void add_callback(const std::string &topic,
	                         std::function<void(const std::string &, const std::string &)> callback);

	static inline bool is_connected() {
		if (_is_initialized) {
			return (xEventGroupGetBitsFromISR(_event_group) & CONNECTED_BIT) != (EventBits_t)0;
		}
		return false;
	}

private:
	static esp_mqtt_client_handle_t _client;
	static EventGroupHandle_t _event_group;
	static std::vector<std::function<void()>> _connection_callbacks;
	static std::multimap<std::string, std::function<void(const std::string &, const std::string &)>> _callbacks;
	static bool _is_initialized;

	static void _event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
};
