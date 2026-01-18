#include <SharpboyCore.h>

//CONSTRUCTOR / DESTRUCTOR
SharpboyCore::SharpboyCore() : m_debug(&m_components) {
	i_Logger = std::make_unique<Logger>();
	set_logging_level(LOGGER_LEVEL_DEBUG);

	m_core_context.initialised = false;

	//LINK COMPONENTS
	if (!m_components.link_components(&m_syncroniser)) { //todo remove syncronisers owenrship of components 
		i_Logger->Log("Failed to link components", LOGGER_PR_ERROR);
		return;
	}

	m_core_context.initialised = true;
	m_core_context.emu_ready = true;

	i_Logger->Log("SharpboyCore instance ready", LOGGER_PR_INFO);
}

SharpboyCore::~SharpboyCore() {
	i_Logger.reset();

	m_components.reset_components();
	i_Logger->Log("SharpboyCore instance destroyed", LOGGER_PR_INFO);
}

bool SharpboyCore::is_initialised() const {
	return m_core_context.initialised;
}

//INSTANCE STARTUP CLEANUP
bool SharpboyCore::initialise_new_instance(std::filesystem::path rom_file_name, bool using_boot_rom) {
	//CHECK FOR ALREADY EXISTING INSTANCE
	if (!m_core_context.emu_ready) {
		return false;
	}

	m_core_context.using_boot_rom = using_boot_rom;

	//READ ROM + OPTIONAL BOOT ROM INTO MEMORY
	std::filesystem::path rom_path = m_core_context.roms_directory / rom_file_name;

	std::vector<u8> rom = std::vector<u8>();
	rom.resize(FileReader::get_file_size(rom_path));

	if (!FileReader::read_file_in_bytes(rom, rom_path)) {
		i_Logger->Log(std::format("Failed to read ROM file : {}", rom_file_name.string()), LOGGER_PR_ERROR);
		return false;
	}

	std::vector<u8> boot_rom = std::vector<u8>();

	if (m_core_context.using_boot_rom) {

		if (!FileReader::check_file_exists(m_core_context.boot_rom_file)) {
			i_Logger->Log("Continuing without boot ROM, could not be read or found", LOGGER_PR_WARNING);
		}
		else {
			boot_rom.resize(FileReader::get_file_size(m_core_context.boot_rom_file));

			if (!FileReader::read_file_in_bytes(boot_rom, m_core_context.boot_rom_file)) {
				m_core_context.using_boot_rom = false;
				i_Logger->Log("Continuing without boot ROM, could not be read or found", LOGGER_PR_WARNING);
			}
		}
	}

	//ASSIGN A CARTRIDGE TYPE FOR THE ROM
	if (!m_components.assign_cart_type(CART_ROM)) { //todo rom only for testing
		i_Logger->Log("Could not create a cartridge object for this ROM!", LOGGER_PR_ERROR);
		return false;
	}

	//INIT COMPONENTS
	m_components.initialise_components(using_boot_rom);
	m_components.load_rom_into_cart(rom, using_boot_rom, boot_rom);

	Interrupts::reset(using_boot_rom);

	m_core_context.emu_ready = false;
	m_core_context.emu_active = true;

	i_Logger->Log(std::format("SharpboyCore instance loaded {} correctly", rom_file_name.string()), LOGGER_PR_INFO);
	return true;
}

void SharpboyCore::cleanup_current_instance() {
	m_components.reset_components();
	Interrupts::reset();

	m_core_context.emu_active = false;
	m_core_context.emu_ready = true;

	i_Logger->Log("SharpboyCore instance cleaned up correctly", LOGGER_PR_INFO);
}

//FILEREADER PATH ADJUSTMENTS
void SharpboyCore::set_roms_directory(std::filesystem::path directory) {
	m_core_context.roms_directory = directory;
	i_Logger->Log(std::format("ROMs directory set to {}", directory.filename().string()), LOGGER_PR_INFO);
}

std::filesystem::path SharpboyCore::get_roms_directory() const {
	return m_core_context.roms_directory;
}

void SharpboyCore::set_boot_rom_file(std::filesystem::path file_name) {
	m_core_context.boot_rom_file = file_name;
	i_Logger->Log(std::format("BOOT ROM file location set to {}", file_name.filename().string()), LOGGER_PR_INFO);
}

std::filesystem::path SharpboyCore::get_boot_rom_file() const {
	return m_core_context.boot_rom_file;
}

//LOGGER LEVEL ADJUSTMENTS
void SharpboyCore::set_logging_level(e_logger_level level) {
	m_core_context.log_level = level;
	i_Logger->set_log_level(level);
	i_Logger->Log("Logger level updated!", LOGGER_PR_DEBUG);
}

e_logger_level SharpboyCore::get_logging_level() const {
	return m_core_context.log_level;
}

//EXECUTION
int SharpboyCore::run() {
	int cycles_advanced = m_syncroniser.advance_cycles();
	return cycles_advanced;
}

//JOYPAD
void SharpboyCore::set_new_joypad_state(s_joypad_state state) {
	m_components.set_joypad_state(state);
}

//DISPLAY
std::array<u32, 160 * 144>* SharpboyCore::get_frame_buffer() {
	return m_components.get_ppu_frame_buffer();
}

bool SharpboyCore::get_frame_ready() {
	return m_components.is_ppu_frame_ready();
}

void SharpboyCore::reset_frame_ready() {
	m_components.ppu_reset_frame_ready();
}

//DEBUG
SBDebug* SharpboyCore::get_debugger() {
	return &m_debug;
}
