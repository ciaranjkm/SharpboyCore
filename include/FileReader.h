#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>

struct s_filereader_paths {
	std::string rom_file_path;
	std::string boot_rom_file_path;

	bool valid_paths = false;
};

class FileReader {
public:
	FileReader(std::string rom_path, std::string boot_rom_path) {
		if (rom_path.empty() || boot_rom_path.empty()) {
			std::cout << "ROM PATH OR BOOT ROM PATH IS EMPTY! EXITING THE PROGRAM...\n";
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
		if (!std::filesystem::exists(rom_path)) {
			std::cout << "FAILED TO FIND ROMS DIRECTORY! ATTEMPTING TO CREATE ONE...\n";

			if (!std::filesystem::create_directory(rom_path)) {
				std::cout << "FAILED TO CREATE ROMS DIRECTORY! CHECK WRITE PERMISSIONS!\nEXITING THE PROGRAM...\n";
				return false;
			}
		}

		std::cout << "FOUND ROMS DIRECTORY\nCHECKING FOR BOOT ROM DIRECTORY...\n";
		if (!std::filesystem::exists(boot_rom_path)) {
			std::cout << "FAILED TO FIND BOOT ROMS DIRECTORY! ATTEMPTING TO CREATE ONE...\n";

			if (!std::filesystem::create_directory(boot_rom_path)) {
				std::cout << "FAILED TO CREATE BOOT ROMS DIRECTORY! CHECK WRITE PERMISSIONS!\nEXITING THE PROGRAM...\n";
				return false;
			}
		}

		std::cout << "FOUND BOOT ROMS DIRECTORY\n";
		return true;
	}
};

static std::unique_ptr<FileReader> fReader = nullptr;
