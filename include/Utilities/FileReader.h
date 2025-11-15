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
	std::string rom_file_path = "";
	std::string boot_rom_file_path = "";
	bool valid_paths = false;

	//NOT REQUIRED, WILL CHECK IF TESTS EXIST SEPERATELY
	std::string sst_path = "";
};

enum e_path_type {
	path_roms,
	path_boot,
	path_sst,
};

class FileReader {
public:
	//GETTERS + SETTERS
	static bool are_paths_valid();
	static void update_paths(std::string roms, std::string boot, std::string sst);

	static void update_path(e_path_type type, std::string path);
	static std::string get_path(e_path_type type);

	static void startup_dir_check();
	static bool check_exists(std::string path);

	//FILE READING
	static bool read_file_bytes(std::vector<u8>& rom_dest, std::string path);

private:
	//MEMBER VARIABLES
	static s_filereader_paths m_filereader_paths;
};

inline s_filereader_paths FileReader::m_filereader_paths = {};
