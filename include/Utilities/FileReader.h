#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <span>

#include "Logger.h"
#include "Common.h"

/*
	Filereader class to use to interact with and read files.
	Used as a static class with public functions.

	Sharpboy will use this to update the paths and read rom files, calling update_paths()
	in the constructor and aborting if the paths are invalid.

	SST testing is done seperately but uses this api, does not use valid_paths and will succeed
	given an invalid path for ssts! This will cause all tests to fail rather than crash.
*/

struct s_filereader_paths {
	std::filesystem::path rom_base_path = "";
	std::filesystem::path boot_rom_path = "";

	std::filesystem::path sst_base_path = "";
};

enum e_path_type {
	path_roms,
	path_boot,
	path_sst,
	path_none,
};

const std::string settings_file_name = "settings.txt";

class FileReader {
public:
	//READ SETTINGS FILE FOR PATHS
	static void read_paths_from_settings();

	//SET SET BASE DIRECTORIES
	static void update_path(e_path_type type, std::filesystem::path new_path);
	static std::filesystem::path get_path(e_path_type type);

	//READ FILE IN BYTES
	static bool read_rom_file(std::vector<u8>& file_dest, std::filesystem::path file_name, bool boot_rom = false);

private:
	//MEMBER VARIABLES
	static s_filereader_paths m_filereader_paths;

private:
	//MEMBER FUNCTIONS
	static bool does_exist(e_path_type path, std::filesystem::path file_name = std::filesystem::path());
};

inline s_filereader_paths FileReader::m_filereader_paths = {};
