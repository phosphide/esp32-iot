#pragma once

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <string>

enum class LogLevel { Debug, Info, Warning, Error, None };

class Logger {
	static LogLevel global_log_level;
	static std::string mqtt_topic;
	static bool mqtt_enabled;

	static int vprintf_esp_message(const char *message, va_list args);
	static void write_message(const std::string &message);

public:
	static void initialize(LogLevel log_level);
	static void initialize(LogLevel log_level, std::string target_mqtt_topic, bool mqtt_logging_enabled = false);
	static void set_mqtt_logging(bool enabled);

	template <typename... Ts> static void log_globally(const Ts &...args) {
		auto message_content = fmt::format(args...);
		write_message(message_content);
	}

	Logger(const std::string &name);

	template <typename... Ts> void debug(const Ts &...args) const {
		if (global_log_level == LogLevel::Debug) {
			auto message_content = fmt::format(args...);
			auto message = fmt::format("{} DEBUG: {}\n", _tag, message_content);
			write_message(message);
		}
	}

	template <typename... Ts> void info(const Ts &...args) const {
		if (global_log_level <= LogLevel::Info) {
			auto message_content = fmt::format(args...);
			auto message = fmt::format("{} INFO: {}\n", _tag, message_content);
			write_message(message);
		}
	}

	template <typename... Ts> void warning(const Ts &...args) const {
		if (global_log_level <= LogLevel::Warning) {
			auto message_content = fmt::format(args...);
			auto message = fmt::format("{} WARNING: {}\n", _tag, message_content);
			write_message(message);
		}
	}

	template <typename... Ts> void error(const Ts &...args) const {
		if (global_log_level <= LogLevel::Error) {
			auto message_content = fmt::format(args...);
			auto message = fmt::format("{} ERROR: {}\n", _tag, message_content);
			write_message(message);
		}
	}

private:
	std::string _tag;
};
