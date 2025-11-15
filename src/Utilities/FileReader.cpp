#include <Utilities/FileReader.h>

bool FileReader::are_paths_valid() {
	return m_filereader_paths.valid_paths;
}

void FileReader::update_paths(std::string roms, std::string boot, std::string sst) {
	bool valid_roms = true;
	bool valid_boot = true;

	valid_roms = check_exists(roms);
	valid_boot = check_exists(boot);

	m_filereader_paths = s_filereader_paths{
		.rom_file_path = roms,
		.boot_rom_file_path = boot,
		.valid_paths = valid_roms && valid_boot,

		.sst_path = sst,
	};
}

void FileReader::update_path(e_path_type type, std::string path) {
	switch (type) {
	case path_roms:
		m_filereader_paths.rom_file_path = path;
		break;

	case path_boot:
		m_filereader_paths.boot_rom_file_path = path;
		break;

	case path_sst:
		m_filereader_paths.sst_path = path;
		return;
	}

	bool valid_roms = true;
	bool valid_boot = true;

	valid_roms = check_exists(m_filereader_paths.rom_file_path);
	valid_boot = check_exists(m_filereader_paths.boot_rom_file_path);
	m_filereader_paths.valid_paths = valid_roms && valid_boot;
}

std::string FileReader::get_path(e_path_type type) {
	switch (type) {
	case path_roms:
		return m_filereader_paths.rom_file_path;

	case path_boot:
		return m_filereader_paths.boot_rom_file_path;

	case path_sst:
		return m_filereader_paths.sst_path;

	default:
		return "";
	}
}

bool FileReader::check_exists(std::string path) {
	if (!std::filesystem::exists(path)) {
		Logger::log(log_error, std::format("Failed to find: {}", path));
		return false;
	}
	else {
		return true;
	}
}

bool FileReader::read_file_bytes(std::vector<u8>& rom_dest, std::string path) {
	if (!check_exists(path)) {
		return false;
	}

	const int file_size = std::filesystem::file_size(path);
	rom_dest.resize(file_size);

	std::ifstream in(path, std::ios::in | std::ios::binary);
	if (!in.is_open()) {
		Logger::log(log_error, std::format("Failed to open file {}", path));
		return false;
	}

	in.read(reinterpret_cast<char*>(rom_dest.data()), rom_dest.size());

	if (!in) {
		Logger::log(log_error, std::format("Failed to read file {} | Expected {} bytes, got {} bytes", path, rom_dest.size(), in.gcount()));
		return false;
	}

	Logger::log(log_debug, std::format("Read file {} | Expected {} bytes, got {} bytes", path, rom_dest.size(), in.gcount()));
	return true;
}