#pragma once

#include <string>
#include <format>
#include <iostream>
#include <memory>

enum e_logger_prefix {
	LOGGER_PR_DEBUG, //DEBUG INFO ON CPU, PPU, APU, ETC
	LOGGER_PR_INFO, //GENERAL INFO STARTUP SHUTDOWN ETC
	LOGGER_PR_WARNING, //WARNING MSG, NON CRITICAL ERRORS 
	LOGGER_PR_ERROR, //CRITICAL ERROR 
	LOGGER_PR_VISUAL //STYLE LOGS FOR VISUAL LIKE COLOURS OR FORMATTING
};

enum e_logger_level {
	LOGGER_LEVEL_NONE,
	LOGGER_LEVEL_BASIC,
	LOGGER_LEVEL_DEBUG
};

/*
	Simple logger class to log messages, includes instance of itself as a unique ptr create it and just include the .h 
	where you need to log from. This could probably be done better with something else but this works for now :/

	Currently the logger is held by core but you can obviously change this.
*/

class Logger {
public:
	inline void set_log_level(e_logger_level new_level) {
		m_current_level = new_level;
	}

	inline void Log(std::string msg, e_logger_prefix prefix = LOGGER_PR_INFO) {
		prepend_prefix(msg, prefix);

		std::cout << msg;
	}

private:
	e_logger_level m_current_level = LOGGER_LEVEL_BASIC;

	inline void prepend_prefix(std::string& msg, e_logger_prefix prefix) {
		switch (prefix) {
		case LOGGER_PR_INFO:
			msg = std::format("[SB:INFO] {}\n", msg);
			return;
		case LOGGER_PR_DEBUG:
			msg = std::format("[SB:DEBUG] {}\n", msg);
			return;
		case LOGGER_PR_WARNING:
			msg = std::format("[SB:WARNING] {}\n", msg);
			return;
		case LOGGER_PR_ERROR:
			msg = std::format("[SB:ERROR] {}\n", msg);
			return;
		default:
			return;
		}
	}
};

static std::unique_ptr<Logger> i_Logger = nullptr;