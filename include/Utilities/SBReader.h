#pragma once

#include <cstdint>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <span>
#include <memory>

#include "SBLogger.h"

class FileReader {
public:
	FileReader() = delete;

	//CHECK IF A FILE EXISTS
	inline static bool check_file_exists(std::filesystem::path path) {
		if (!std::filesystem::exists(path)) {
			return false;
		}

		return true;
	}

	//CHECK IF A FILE IS A REGULAR FILE NOT A DIR ETC
	inline static bool check_file_is_regular(std::filesystem::path path) {
		if(!std::filesystem::is_regular_file(path)) {
			return false;
		}

		return true;
	}

	//GET FILE SIZE
	inline static size_t get_file_size(std::filesystem::path path) {
		return std::filesystem::file_size(path);
	}

	//GET FILE NAME 
	inline static std::string get_file_name(std::filesystem::path path) {
		return path.filename().string();
	}

	//GET FILE EXTENSION
	inline static std::string get_file_extension(std::filesystem::path path) {
		return path.extension().string();
	}

	//READ FILE IN BYTES INTO A SPAN
	inline static bool read_file_in_bytes(std::span<uint8_t> buffer, std::filesystem::path path) {
		if (!check_file_exists(path) || !check_file_is_regular(path)) {
			i_Logger->Log("File does not exist or is not a regular file", LOGGER_PR_DEBUG);
			return false;
		}

		size_t file_size = get_file_size(path);
		if (buffer.size() < file_size) {
			i_Logger->Log("Buffer size is invalid for this ROM file", LOGGER_PR_DEBUG);
			return false;
		}

		std::fstream in{ path, std::ios::in | std::ios::binary };
		if (!in.is_open()) {
			i_Logger->Log("File could not be opened", LOGGER_PR_DEBUG);
			return false;
		}

		in.read(reinterpret_cast<char*>(buffer.data()), file_size);
		in.close();

		i_Logger->Log("File read success", LOGGER_PR_DEBUG);
		i_Logger->Log(std::format("Name: {}", get_file_name(path)), LOGGER_PR_DEBUG);
		i_Logger->Log(std::format("Size: {} bytes", get_file_size(path)), LOGGER_PR_DEBUG);

		return true;
	}
};