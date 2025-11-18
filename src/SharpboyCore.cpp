#include <SharpboyCore.h>

//STARTUP LOGGER AND FILEREADER
//ATTACH COMPONENTS TO EACHOTHER AND SET INIT FLAG
SharpboyCore::SharpboyCore(std::string roms_path, std::string boot_rom_path, std::string sst_path, bool status_out, bool debug_out) {
	Logger::update_logger(status_out, debug_out);
	
	FileReader::update_paths(roms_path, boot_rom_path, sst_path);
	if (!FileReader::are_paths_valid()) {
		m_core_context.initialised = false;
		return;
	}

	//LINK COMPONENTS THAT DONT GET DELETED IN COMP MANAGER
	m_components.link_components();
	if (!m_components.is_linked()) {
		m_core_context.initialised = false;
		return;
	}

	//ADD SYNCRONISER TO CPU AND ADD COMPONENTS TO SYNCRONISER
	m_components.add_syncroniser(&m_syncroniser);
	m_syncroniser.attach_components(&m_components);

	m_core_context.initialised = true;
	m_core_context.emu_ready = true;
	Logger::log(log_default, "SharpboyCore created successfully");
}

//DESTROY POINTERS AND CLEANUP
SharpboyCore::~SharpboyCore() {
	m_components.reset_components();

	Logger::log(log_default, "Core destroyed successfully");
}

//GETTERS
bool SharpboyCore::is_initialised() const {
	return m_core_context.initialised;
}

//INITIALISATION FOR SHARPBOY, ON RE-INIT MAKE SURE TO CALL CLEANUP
bool SharpboyCore::emu_init(std::string rom_file_name, bool using_boot_rom) {
	m_core_context.using_boot_rom = using_boot_rom;
	Logger::log(log_status, "Creating new instance");
	if (!m_core_context.emu_ready) {
		Logger::log(log_error, "Instance not ready! Did you call cleanup?");
		return false;
	}

	//READ ROM FILE INTO MEMORY
	std::vector<u8> rom = std::vector<u8>();
	if (!read_rom_file(rom, rom_file_name, m_core_context.using_boot_rom ? false : false)) { //jank to force rom
		rom.clear();
		return false;
	}

	//READ BOOT ROM INTO MEMORY [ should use array ]
	std::vector<u8> boot_rom = std::vector<u8>();
	if (m_core_context.using_boot_rom) {
		if (!read_rom_file(boot_rom, "BOOT.bin", m_core_context.using_boot_rom)) {
			boot_rom.clear();
			m_core_context.using_boot_rom = false;

			Logger::log(log_error, "Running without BOOT ROM");
		}
	}

	//INIT FIRST TO CLEAR PREVIOUS CART
	m_components.initialise_components(using_boot_rom);

	//CREATE NEW CARTRIDGE OBJECT AND LOAD ROM
	if (!m_components.assign_cart_type(CART_ROM)) {
		Logger::log(log_error, "Failed to create new cartridge object");

		rom.clear();
		boot_rom.clear();
		return false;
	}
	m_components.load_rom_into_cart(rom, using_boot_rom, boot_rom);

	Logger::log(log_status, "Instance ready");
	return true;
}

//CALL WHEN CLOSING AN EMULATOR INSTANCE
void SharpboyCore::cleanup() {
	m_components.reset_components();

	m_core_context.emu_ready = true;
	Logger::log(log_status, "Cleanup successful, ready for new instance");
}

//RUN TO BE CALLED IN THE MAIN LOOP
void SharpboyCore::run() {
	int cycles_advanced = m_syncroniser.advance_cycles();
	std::this_thread::sleep_for(std::chrono::nanoseconds(10));
}

//DEBUG + SST
/*
	EXECUTE ALL SINGLE STEP TESTS FOR NORMAL AND PREFIXED OPCODES (HALT, STOP, ILLEGAL NOT INCL.) PROOF OF CONCEPT REALLY
	USES THE SST PATH IN FILEREADER STATIC OBJECT, MAKE SURE IT IS UPDATED BEFORE RUNNING OR ALL FAILS
*/
void SharpboyCore::run_ssts(bool show_all_results, bool prefixed) {
	SST sst(FileReader::get_path(path_sst), 0x00, prefixed);

	std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

	std::thread sst_thread = std::thread([&sst]() {sst.run(); });
	sst_thread.detach();

	while (!sst.is_test_complete()) {
		float progess = (((float)(sst.get_completed_tests_count()) / SMALL_TEST_COUNT) * 100.0f);
		std::string msg = std::format("Completing SSTs, progess: {}%", (int)progess);
		Logger::log(log_sst_status, msg);

		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}

	if (sst_thread.joinable()) {
		sst_thread.join();
	}

	std::chrono::duration<float> elapsed_time = std::chrono::high_resolution_clock::now() - start_time;

	std::string msg = std::format("SSTs complete, time taken: {}s", elapsed_time.count());
	Logger::log(log_sst_status, msg);

	std::array<s_test_result, SMALL_TEST_COUNT>* results = sst.get_results();
	std::vector<std::string> output = std::vector<std::string>();

	//ACCOUNT FOR HALT STOP CB AND ILLEGALS IN FIRST OPCODE TABLE
	int failed_tests = prefixed ? 0 : -14;
	int tests_completed = prefixed ? 0 : -14;

	for (s_test_result result : *results) {
		if (!result.result) {
			output.push_back(std::format("{}", result.msg));
			failed_tests++;
		}
		else if (show_all_results) {
			output.push_back(std::format("{}", result.msg));
		}

		tests_completed++;
	}

	Logger::log(log_sst_status, std::format("Tests completed: {} | Tests failed: {}", tests_completed, failed_tests));
	for (int i = 0; i < output.size(); i++) {
		Logger::log(log_sst_status, output[i]);
	}
	results = nullptr;
}

s_core_context* SharpboyCore::get_core_context() {
	return &m_core_context;
}

//MEMBER FUNCTIONS
bool SharpboyCore::read_rom_file(std::vector<u8>& rom, std::string file_name, bool boot_rom) {
	e_path_type base_path = boot_rom ? path_boot : path_roms;
	std::string rom_path = FileReader::get_path(base_path) + "/" + (boot_rom ? "BOOT.bin" : file_name);

	bool read_rom = FileReader::read_file_bytes(rom, rom_path);
	if (!read_rom) {
		return false;
	}

	return true;
}