/*
 * utils.hpp
 *
 *  Created on: 23 paź 2019
 *      Author: phosphide
 */

#pragma once

#include <experimental/optional>

template<class T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi ) { // almost C++17
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

inline std::experimental::optional<float> stof_check(const std::string& input) {
	size_t good_bytes;
	auto output = std::stof(input, &good_bytes);
	
	if (good_bytes == input.size()) {
		return output;
	}
	
	return {};
}
