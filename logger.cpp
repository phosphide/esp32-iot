#include "logger.hpp"

#include <cstdio>
#include <iostream>

#include <esp_log.h>

#include "mqtt.hpp"

LogLevel Logger::global_log_level = LogLevel::Debug;
std::string Logger::mqtt_topic;
bool Logger::mqtt_enabled = false;

void Logger::initialize(LogLevel log_level) {
	global_log_level = log_level;
	esp_log_set_vprintf(vprintf_esp_message);
}

void Logger::initialize(LogLevel log_level, std::string target_mqtt_topic, bool mqtt_logging_enabled) {
	initialize(log_level);
	mqtt_topic = std::move(target_mqtt_topic);
	mqtt_enabled = mqtt_logging_enabled;
}

void Logger::set_mqtt_logging(bool enabled) {
	mqtt_enabled = enabled;
}

int Logger::vprintf_esp_message(const char *message, va_list args) {
	auto length = std::vsnprintf(nullptr, 0, message, args);
	std::string string_message(length + 1, '\0');
	auto characters_written = std::vsprintf(&string_message[0], message, args);

	write_message(fmt::format("[ESP] {}", string_message));

	return characters_written;
}

void Logger::write_message(const std::string &message) {
	std::cout << message;
	if (mqtt_enabled) {
		MQTTClient::publish(mqtt_topic, message);
	}
}

Logger::Logger(const std::string &name) : _tag("[" + name + "]") {
}
