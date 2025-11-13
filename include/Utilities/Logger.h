#pragma once

#include <format>
#include <iostream>

struct s_logger {
	bool status = false;
	bool debug = false;
};

static s_logger logger;

static inline void update_logger(bool status, bool debug) {
	logger.status = status;
	logger.debug = debug;
}

static inline void msg_error(std::string message) {
	std::string msg = std::format("[CORE::ERROR] {}\n", message);
	std::cout << msg;
}

static inline void msg_default(std::string message) {
	std::string msg = std::format("[CORE] {}\n", message);
	std::cout << msg;
}

static inline void msg_debug(std::string message) {
	if (!logger.debug) {
		return;
	}

	std::string msg = std::format("[CORE::DEBUG] {}\n", message);
	std::cout << msg;
}

static inline void msg_status(std::string message) {
	if (!logger.status) {
		return;
	}

	std::string msg = std::format("[CORE] {}\n", message);
	std::cout << msg;
}

static inline void msg_sst_error(std::string message) {
	std::string msg = std::format("[C/SST:ERROR] {}\n", message);
	std::cout << msg;
}

static inline void msg_sst_status(std::string message) {
	std::string msg = std::format("[C/SST] {}\n", message);
	std::cout << msg;
}

static inline void msg_sst_progress(bool prefix, int test_number, int test_total) {
	float complete_per = ((float)test_number / (float)test_total) * 100.0f;
	std::string msg = std::format("[C/SST] Progress : {}% Test Number: {}\n", (int)complete_per, test_number);
	std::cout << msg;
}