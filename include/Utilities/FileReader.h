#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>

#include "Logger.h"

struct s_filereader_paths {
	std::string rom_file_path;
	std::string boot_rom_file_path;

	bool valid_paths = false;
};

class FileReader {
public:
	FileReader(std::string rom_path, std::string boot_rom_path) {
		if (rom_path.empty() || boot_rom_path.empty()) {
			Logger::log(log_error, "Rom path or boot rom path not found");
			return;
		}

		m_filereader_paths.valid_paths = roms_directory_check(rom_path, boot_rom_path);

		if (m_filereader_paths.valid_paths) {
			m_filereader_paths.rom_file_path = rom_path;
			m_filereader_paths.boot_rom_file_path = boot_rom_path;
		}
	}

	inline bool is_initialied() const {
		return m_filereader_paths.valid_paths;
	}

private:
	s_filereader_paths m_filereader_paths;

private:
	//CHECK IF ROMS DIR EXISTS AND ATTEMPT TO CREATE IT, RETURN TRUE IF DIR FOUND
	inline bool roms_directory_check(std::string rom_path, std::string boot_rom_path) {
		Logger::log(log_debug, "Checking ROM directory exists");
		if (!std::filesystem::exists(rom_path)) {
			Logger::log(log_error, "Failed finding ROM directory. Creating one");

			if (!std::filesystem::create_directory(rom_path)) {
				Logger::log(log_error, "Failed to create a new ROM directory");
				return false;
			}
		}

		Logger::log(log_debug, "Found ROM directory. Checking BOOT ROM directory");
		if (!std::filesystem::exists(boot_rom_path)) {
			Logger::log(log_error, "Failed finding BOOT ROM directory. Creating one");

			if (!std::filesystem::create_directory(boot_rom_path)) {
				Logger::log(log_error, "Failed to create a new BOOT ROM directory");
				return false;
			}
		}

		Logger::log(log_debug, "FOUND BOOT ROMS DIRECTORY");
		return true;
	}
};

static std::unique_ptr<FileReader> fReader = nullptr;
