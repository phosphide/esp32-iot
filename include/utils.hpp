/*
 * utils.hpp
 *
 *  Created on: 23 paź 2019
 *      Author: phosphide
 */

#pragma once

#include <optional>

#include "logger.hpp"

inline std::optional<float> stof_check(const std::string &input) {
	size_t good_bytes;
	auto output = std::stof(input, &good_bytes);
	if (good_bytes == input.size()) {
		return output;
	}
	return {};
}

inline std::optional<int> stoi_check(const std::string &input) {
	size_t good_bytes;
	auto output = std::stoi(input, &good_bytes);
	if (good_bytes == input.size()) {
		return output;
	}
	return {};
}

inline void log_if_error(const std::error_code &ec, const std::string &message, const Logger &logger) {
	if (ec) {
		logger.error(message, ec.message());
	}
}

inline void log_if_error(const std::error_code &ec, const std::string &message) {
	static Logger logger("APP");

	log_if_error(ec, message, logger);
}

class Prescaler {
public:
	Prescaler(float input_min, float input_max, float output_min, float output_max, float output_off)
	    : input_min_(input_min), input_max_(input_max), output_min_(output_min), output_max_(output_max),
	      output_off_(output_off) {
	}
	float operator()(float input) const {
		if (input == input_min_) {
			return output_off_;
		}
		return (std::clamp(input, input_min_, input_max_) - input_min_) / (input_max_ - input_min_) *
		           (output_max_ - output_min_) +
		       output_min_;
	}

private:
	const float input_min_;
	const float input_max_;
	const float output_min_;
	const float output_max_;
	const float output_off_;
};