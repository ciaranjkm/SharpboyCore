#include <SharpboyCore.h>

SharpboyCore::SharpboyCore() {
	m_core_context.initialised = false;

	//LINK COMPONENTS
	if (!m_components.link_components(&m_syncroniser)) {
		m_current_error = { error_fatal, "Component link failed" };
		return;
	}

	m_core_context.initialised = true;
	m_core_context.emu_ready = true;
}

SharpboyCore::~SharpboyCore() {
	m_components.reset_components();
}

bool SharpboyCore::is_initialised() const {
	return m_core_context.initialised;
}

std::string SharpboyCore::get_error() const {
	return m_current_error.get_error();
}

void SharpboyCore::output_current_error() {
	m_current_error.output();
}

bool SharpboyCore::initialise_new_instance(std::filesystem::path rom_file_name, bool using_boot_rom) {
	//CHECK FOR ALREADY EXISTING INSTANCE
	if (!m_core_context.emu_ready) {
		return false;
	}

	m_core_context.using_boot_rom = using_boot_rom;

	//READ ROM + OPTIONAL BOOT ROM INTO MEMORY
	std::vector<u8> rom = std::vector<u8>();
	std::vector<u8> boot_rom = std::vector<u8>();
	if (!FileReader::read_rom_file(rom, rom_file_name)){
		return false;
	}
	if (m_core_context.using_boot_rom) {
		if (!FileReader::read_rom_file(boot_rom, "BOOT.bin", true)) {
			m_core_context.using_boot_rom = false;
		}
	}

	//ASSIGN A CARTRIDGE TYPE FOR THE ROM
	if (!m_components.assign_cart_type(CART_ROM)) { //todo rom only for testing
		m_current_error = { error_initialisation, "Could not create cartridge object for this ROM" };
		return false;
	}

	//INIT COMPONENTS
	m_components.initialise_components(using_boot_rom);
	m_components.load_rom_into_cart(rom, using_boot_rom, boot_rom);

	//START SYNCRONISER TIMER
	m_syncroniser.start_syncroniser();

	m_core_context.emu_ready = false;
	m_core_context.emu_active = true;
	return true;
}

void SharpboyCore::cleanup_current_instance() {
	m_components.reset_components();

	m_core_context.emu_active = false;
	m_core_context.emu_ready = true;
}

int SharpboyCore::run() {
	int cycles_advanced = m_syncroniser.advance_cycles();
	return cycles_advanced;
}

void SharpboyCore::run_ssts(bool show_all_results, bool prefixed) {
	SST sst(FileReader::get_path(path_sst), prefixed);

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

	std::string msg = std::format("SSTs complete, time taken: {}s", elapsed_time.count());
	Logger::log(log_sst_status, msg);
	Logger::log(log_sst_status, std::format("Tests completed: {} | Tests failed: {}", tests_completed, failed_tests));
	for (int i = 0; i < output.size(); i++) {
		Logger::log(log_sst_status, output[i]);
	}
	results = nullptr;
}

s_core_context* SharpboyCore::get_core_context() {
	return &m_core_context;
}
