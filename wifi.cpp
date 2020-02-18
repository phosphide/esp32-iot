#include "wifi.hpp"

#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <esp_event_loop.h>

#include "errors.hpp"

Logger WiFi::LOGGER("WiFi");
EventGroupHandle_t WiFi::_event_group;

std::error_code WiFi::initialize() {
	LOGGER.debug("Initializing");
	esp_log_level_set("wifi", ESP_LOG_NONE);

	_event_group = xEventGroupCreate();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	RETURN_IF_ERROR( esp_wifi_init(&cfg) );

	RETURN_IF_ERROR( esp_event_loop_init(_event_handler, NULL) );

	RETURN_IF_ERROR( esp_wifi_set_mode(WIFI_MODE_STA) );
	RETURN_IF_ERROR( esp_wifi_start() );
	RETURN_IF_ERROR( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

	return {};
}

std::error_code WiFi::connect(const std::string &ssid, const std::string &password, TickType_t timeout) {
	wifi_config_t wifi_config = {
		.sta = {}
	};
	ssid.copy(reinterpret_cast<char *>(wifi_config.sta.ssid), ssid.size() + 1);
	password.copy(reinterpret_cast<char *>(wifi_config.sta.password), password.size() + 1);

	RETURN_IF_ERROR( esp_wifi_disconnect() );
	RETURN_IF_ERROR( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
	RETURN_IF_ERROR( esp_wifi_connect() );

	EventBits_t eventgroup_bits = xEventGroupWaitBits(_event_group, CONNECTED_BIT, true, true, timeout); // timeout after 30s
	if ((eventgroup_bits & CONNECTED_BIT) != CONNECTED_BIT) {
		return to_strong_error(ESP_ERR_TIMEOUT);
	}

	return {};
}

void WiFi::on_got_ip() {
   xEventGroupSetBits(_event_group, CONNECTED_BIT);
}

std::error_code WiFi::on_disconnect() {
	RETURN_IF_ERROR( esp_wifi_connect() );
	return {};
}

esp_err_t WiFi::_event_handler(void *ctx, system_event_t *event) {
	switch(event->event_id) {
	case SYSTEM_EVENT_STA_START: {
		return esp_wifi_connect();
	}
	case SYSTEM_EVENT_STA_GOT_IP: {
		xEventGroupSetBits(_event_group, CONNECTED_BIT);
		break;
	}
	case SYSTEM_EVENT_STA_DISCONNECTED: {
		/* This is a workaround as ESP32 WiFi libs don't currently
		auto-reassociate. */
		auto err = esp_wifi_connect();
		if (err == ESP_OK) {
			xEventGroupClearBits(_event_group, CONNECTED_BIT);
		}
		return err;
	}
	default:
		break;
	}

	return ESP_OK;
}
