#pragma once

#include <format>
#include <iostream>

static inline void msg_error(std::string message) {
	std::string error_msg = std::format("[CORE::ERROR] {}\n", message);
	std::cout << error_msg;
}

static inline void msg_debug(std::string message) {
	std::string debug_msg = std::format("[CORE::DEBUG] {}\n", message);
	std::cout << debug_msg;
}

static inline void msg_status(std::string message) {
	std::string msg = std::format("[CORE] {}\n", message);
	std::cout << msg;
}