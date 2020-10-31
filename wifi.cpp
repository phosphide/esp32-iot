#include "wifi.hpp"

#include <esp_event.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>

#include "errors.hpp"

Logger WiFi::LOGGER("WiFi");
EventGroupHandle_t WiFi::_event_group;
bool WiFi::_is_initialized = false;

std::error_code WiFi::initialize() {
	LOGGER.debug("Initializing");
	esp_log_level_set("wifi", ESP_LOG_WARN);

	_event_group = xEventGroupCreate();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	RETURN_IF_ERROR(esp_wifi_init(&cfg));

	RETURN_IF_ERROR(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, _event_handler, nullptr));
	RETURN_IF_ERROR(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, _on_got_ip, nullptr));

	RETURN_IF_ERROR(esp_wifi_set_mode(WIFI_MODE_STA));
	RETURN_IF_ERROR(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	_is_initialized = true;
	return {};
}

std::error_code WiFi::connect(const std::string &ssid, const std::string &password, TickType_t timeout) {
	wifi_config_t wifi_config = {.sta = {}};
	ssid.copy(reinterpret_cast<char *>(wifi_config.sta.ssid), ssid.size() + 1);
	password.copy(reinterpret_cast<char *>(wifi_config.sta.password), password.size() + 1);

	RETURN_IF_ERROR(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	RETURN_IF_ERROR(esp_wifi_start());

	EventBits_t eventgroup_bits =
	    xEventGroupWaitBits(_event_group, CONNECTED_BIT, true, true, timeout); // timeout after 30s
	if ((eventgroup_bits & CONNECTED_BIT) != CONNECTED_BIT) {
		return to_strong_error(ESP_ERR_TIMEOUT);
	}

	return {};
}

void WiFi::_on_got_ip(void * /*ctx*/, esp_event_base_t /*event_base*/, int32_t /*event_id*/, void * /*event_data*/) {
	LOGGER.debug("GOT_IP");
	xEventGroupSetBits(_event_group, CONNECTED_BIT);
}

void WiFi::_event_handler(void * /*ctx*/, esp_event_base_t /*event_base*/, int32_t event_id, void * /*userdata*/) {
	switch (event_id) {
	case WIFI_EVENT_STA_START: {
		LOGGER.debug("STA_START");
		esp_wifi_connect();
		break;
	}
	case WIFI_EVENT_STA_DISCONNECTED: {
		LOGGER.debug("STA_DISCONNECTED");
		/* This is a workaround as ESP32 WiFi libs don't currently
		auto-reassociate. */
		auto err = esp_wifi_connect();
		if (err == ESP_OK) {
			xEventGroupClearBits(_event_group, CONNECTED_BIT);
		}
	}
	default:
		break;
	}
}
