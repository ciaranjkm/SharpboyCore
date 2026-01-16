#pragma once

#include <string>
#include <iostream>

enum e_logger_level {
	LOGGER_LV_DEBUG, //DEBUG INFO ON CPU, PPU, APU, ETC
	LOGGER_LV_INFO, //GENERAL INFO STARTUP SHUTDOWN ETC
	LOGGER_LV_WARNING, //WARNING MSG, NON CRITICAL ERRORS 
	LOGGER_LV_ERROR, //CRITICAL ERROR 
	LOGGER_LV_VISUAL //STYLE LOGS FOR VISUAL LIKE COLOURS OR FORMATTING
};

class Logger {
public:
	Logger() = delete;

	//SIMPLE LOGGING FUNCTION
	inline static void Log(const std::string& msg, e_logger_level level = LOGGER_LV_DEBUG) {
		std::string out = "";

		switch (level) {
		case LOGGER_LV_INFO:
			out.append("[SB:INFO] ");
			break;

		case LOGGER_LV_DEBUG:
			out.append("[SB:DEBUG] ");
			break;

		case LOGGER_LV_WARNING:
			out.append("[SB:WARNING] ");
			break;

		case LOGGER_LV_ERROR:
			out.append("[SB:ERROR] ");
			break;

		default:
			break;
		}

		out.append(msg);
		out.append("\n");

		std::cout << out;
	}
};