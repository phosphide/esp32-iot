#pragma once

#include <fmt/format.h>
#include <iostream>

enum class LogLevel { Debug, Info, Warning, Error, None };

class Logger {
	static LogLevel global_log_level;

public:
	static void initialize(LogLevel log_level);

	Logger(const std::string &name);

	template <typename... Ts> void debug(const Ts &... args) const {
		if (global_log_level == LogLevel::Debug) {
			std::cout << _tag << " DEBUG: " << fmt::format(args...) << "\n";
		}
	}

	template <typename... Ts> void info(const Ts &... args) const {
		if (global_log_level <= LogLevel::Info) {
			std::cout << _tag << " INFO: " << fmt::format(args...) << "\n";
		}
	}

	template <typename... Ts> void warning(const Ts &... args) const {
		if (global_log_level <= LogLevel::Warning) {
			std::cout << _tag << " WARNING: " << fmt::format(args...) << "\n";
		}
	}

	template <typename... Ts> void error(const Ts &... args) const {
		if (global_log_level <= LogLevel::Error) {
			std::cout << _tag << " ERROR: " << fmt::format(args...) << "\n";
		}
	}

private:
	std::string _tag;
};
