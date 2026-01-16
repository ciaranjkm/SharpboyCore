#pragma once

#include <string>
#include <iostream>

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

class Logger {
public:
	Logger() = delete;

	//SIMPLE LOGGING FUNCTION
	inline static void Log(const std::string& msg, e_logger_level current_level, e_logger_prefix prefix = LOGGER_PR_DEBUG) {
		if (current_level == LOGGER_LEVEL_NONE) {
			return;
		}
		
		std::string out = "";

		switch (prefix) {
		case LOGGER_PR_INFO:
			out.append("[SB:INFO] ");
			break;

		case LOGGER_PR_WARNING:
			out.append("[SB:WARNING] ");
			break;

		case LOGGER_PR_ERROR:
			out.append("[SB:ERROR] ");
			break;

		case LOGGER_PR_DEBUG:
			if (current_level != LOGGER_LEVEL_DEBUG) {
				return;
			}
			out.append("[SB:DEBUG] ");
			break;

		default:
			break;
		}

		out.append(msg);
		out.append("\n");

		std::cout << out;
	}
};