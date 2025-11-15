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

	//set pointers for cpu 
	m_cpu.set_bus_ptr(&m_bus);
	m_cpu.set_timing_ptr(&m_timing);

	//attach components to the timing manager :: todo this will sync the emulator to the audio buffer eventually
	//gather up cycles for a second of audio then play the audio while gathering the next second
	m_timing.attach_components(&m_cpu, &m_ppu);

	//attach components to the bus
	m_bus.update_imu_ptr(&m_imu);
	m_bus.update_ppu_ptr(&m_ppu);

	//init and ready to start new emu instance
	m_core_context.initialised = true;
	m_core_context.emu_ready = true;

	Logger::log(log_default, "SharpboyCore created successfully");
}

//DESTROY POINTERS AND CLEANUP
SharpboyCore::~SharpboyCore() {
	//delete cartridge object
	m_cartridge.reset();

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

	//CREATE NEW CARTRIDGE OBJECT AND LOAD ROM
	if (!create_new_mbc(CART_ROM, 0, 0)) {
		Logger::log(log_error, "Failed to create new cartridge object");
		rom.clear();
		return false;
	}

	if (m_core_context.using_boot_rom) {
		if (!m_cartridge->load_boot_rom(boot_rom)) {
			m_core_context.using_boot_rom = false;
			Logger::log(log_error, "Running without BOOT ROM");
		}
	}
	m_cartridge->load_rom(rom);
	if (m_core_context.using_boot_rom) {
		m_cartridge->swap_boot_rom_buffer();
	}


	//UPDATE BUS PTR TO CART ON REINIT TO STOP NULLPTR
	m_bus.update_cartridge_ptr(m_cartridge.get());

	//RESET CPU
	m_cpu.reset(m_core_context.using_boot_rom);

	Logger::log(log_status, "Instance ready");
	return true;
}

//CALL WHEN CLOSING AN EMULATOR INSTANCE
void SharpboyCore::cleanup() {
	//reset components and destroy cart object
	m_cartridge.reset();

	m_bus.update_cartridge_ptr(nullptr);

	m_cpu.reset(false);
	m_imu.reset();

	m_core_context.emu_ready = true;
	Logger::log(log_status, "Cleanup successful, ready for new instance");
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

bool SharpboyCore::create_new_mbc(const e_cart_types cart_type, u8 rom_size, u8 ram_size) {
	m_cartridge.reset(); //destroy old object just incase

	switch (cart_type) {
	case CART_ROM:
		m_cartridge = std::make_unique<CartMBC0>();
		break;

	case CART_ROM_RAM:
		m_cartridge = std::make_unique<CartMBC0>(cart_type, true, false);
		break;

	case CART_ROM_RAM_BATTERY:
		m_cartridge = std::make_unique<CartMBC0>(cart_type, true, true);
		break;

	default:
		return false;
	}

	if (!m_cartridge) {
		return false;
	}

	return true;
}