#include "logger.hpp"

#include <iostream>

LogLevel Logger::global_log_level = LogLevel::Debug;

void Logger::initialize(LogLevel log_level) {
    global_log_level = log_level;
}

Logger::Logger(const std::string &name) : _tag("[" + name + "]") {
}
