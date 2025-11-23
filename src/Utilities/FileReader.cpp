#include <Utilities/FileReader.h>

void FileReader::update_path(e_path_type type, std::filesystem::path new_path) {
	//DONT CHECK FOR VALIDITY, ALLOW EMPTY PATH 
	switch (type) {
	case path_roms:
		m_filereader_paths.rom_base_path = new_path;
		break;

	case path_boot:
		m_filereader_paths.boot_rom_path = new_path;
		break;

	case path_sst:
		m_filereader_paths.sst_base_path = new_path;
		break;

	default: break;
	}
}

std::filesystem::path FileReader::get_path(e_path_type type) {
	switch (type) {
	case path_roms:
		return m_filereader_paths.rom_base_path;

	case path_boot:
		return m_filereader_paths.boot_rom_path;

	case path_sst:
		return m_filereader_paths.sst_base_path;

	default: return std::filesystem::path();
	}
}

//READ FILE IN BYTES
bool FileReader::read_rom_file(std::vector<u8>& file_dest, std::filesystem::path file_name, bool boot_rom) {
	if (!does_exist((boot_rom ? path_boot : path_roms), file_name)) {
		return false;
	}

	//GET ABSOLUTE PATH OF THE ROM FILE IF IT EXISTS
	std::filesystem::path rom_file_path = (boot_rom ? get_path(path_boot) : get_path(path_roms)) / file_name;
	
	//READ FILE SIZE IN BYTES
	const size_t file_size = std::filesystem::file_size(rom_file_path);

	//CHECK FILE SIZE FOR BOOT ROM 0x100
	if (boot_rom) {
		if (file_size != 0x100) {
			return false;
		}
	}

	//RESIZE DESTINATION VECTOR TO FILE SIZE
	file_dest.resize(file_size);

	//OPEN THE ROM FILE
	std::ifstream in(rom_file_path, std::ios::in | std::ios::binary);
	if (!in.is_open()) {
		return false;
	}

	//INPUT FILE IS INVALID FOR SOME REASON AFTER OPENING
	if (!in) {
		return false;
	}

	//READ THE BYTES INTO THE FILE DESTINATION
	in.read(reinterpret_cast<char*>(file_dest.data()), file_size);
	return true;
}

//MEMBER FUNCTIONS
bool FileReader::does_exist(e_path_type path, std::filesystem::path file_name) {
	std::filesystem::path to_check = get_path(path);
	to_check /= file_name;

	if (!std::filesystem::exists(to_check)) {
		return false;
	}
	else {
		return true;
	}
}
