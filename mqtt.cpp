#include "mqtt.hpp"
#include "errors.hpp"

Logger MQTTClient::LOGGER("MQTTClient");
esp_mqtt_client_handle_t MQTTClient::_client;
EventGroupHandle_t MQTTClient::_event_group;
std::function<void()> MQTTClient::_on_connection;
std::multimap<std::string, std::function<void (const std::string &, const std::string &)>> MQTTClient::_callbacks;


std::error_code MQTTClient::initialize(const std::string &client_id, const std::string &broker_address, TickType_t timeout) {
    LOGGER.debug("Initializing");
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_NONE);

    _event_group = xEventGroupCreate();

    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.event_handle = _event_handler;
    mqtt_cfg.uri = broker_address.c_str();
    mqtt_cfg.client_id = client_id.c_str();

    _client = esp_mqtt_client_init(&mqtt_cfg);
    if (_client == nullptr) {
        LOGGER.error("Error initializing client");
        return RuntimeError::MQTTInitializationFailed;
    }

    LOGGER.debug("Starting client");
    RETURN_IF_ERROR( esp_mqtt_client_start(_client) );

    xEventGroupWaitBits(_event_group, CONNECTED_BIT, true, true, timeout);
    return {};
}

std::error_code MQTTClient::publish(const std::string &topic, const std::string &data, bool retain) {
	if (esp_mqtt_client_publish(_client, topic.c_str(), data.c_str(), data.size(), 1,  static_cast<int>(retain)) == -1) {
		return RuntimeError::MQTTPublishFailed;
	}

	return {};
}

std::error_code MQTTClient::subscribe(const std::string &topic) {
	RETURN_IF_ERROR( esp_mqtt_client_subscribe(_client, topic.c_str(), 1) );
	return {};
}

void MQTTClient::add_connection_callback(std::function<void()> callback) {
	_on_connection = std::move(callback);
}

void MQTTClient::add_callback(const std::string &topic, std::function<void (const std::string &, const std::string &)> callback) {
	_callbacks.emplace(topic, callback);
}

esp_err_t MQTTClient::_event_handler(esp_mqtt_event_handle_t event)  {
	auto client = event->client;
	// your_context_t *context = event->context;
	switch (event->event_id) {
		case MQTT_EVENT_CONNECTED: {
			LOGGER.info("MQTT_EVENT_CONNECTED");
			xEventGroupSetBits(_event_group, CONNECTED_BIT);
			break;
		}
		case MQTT_EVENT_DISCONNECTED: {
			LOGGER.info("MQTT_EVENT_DISCONNECTED");
			xEventGroupClearBits(_event_group, CONNECTED_BIT);
			esp_mqtt_client_start(_client);
			break;
		}

		case MQTT_EVENT_SUBSCRIBED: {
			LOGGER.info("MQTT_EVENT_SUBSCRIBED, msg_id={}", event->msg_id);
			break;
		}
		case MQTT_EVENT_UNSUBSCRIBED: {
			LOGGER.info("MQTT_EVENT_UNSUBSCRIBED, msg_id={}", event->msg_id);
			break;
		}
		case MQTT_EVENT_PUBLISHED: {
			LOGGER.info("MQTT_EVENT_PUBLISHED, msg_id={}", event->msg_id);
			break;
		}
		case MQTT_EVENT_DATA: {
			std::string topic(event->topic, 0, event->topic_len);
			std::string data(event->data, 0, event->data_len);
			LOGGER.info("MQTT_EVENT_DATA");
			LOGGER.debug("TOPIC=" + topic);
			LOGGER.debug("DATA=" + data);
			auto matches = _callbacks.equal_range(topic);
			for (auto it = matches.first; it != matches.second; ++it) {
				LOGGER.debug("callback match");
				it->second(topic, data);
			}
			break;
		}
		case MQTT_EVENT_ERROR: {
			LOGGER.info("MQTT_EVENT_ERROR");
			break;
		}
		default: {
			LOGGER.info("Other event id: {}", event->event_id);
			break;
		}
	}
	return ESP_OK;
}
