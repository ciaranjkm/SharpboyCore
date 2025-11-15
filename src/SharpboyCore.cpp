#include <SharpboyCore.h>

//INITIALISE FILEREADER AND OTHER DEPS
SharpboyCore::SharpboyCore(std::string roms_path, std::string boot_rom_path, bool status_out, bool debug_out) {
	Logger::update_logger(status_out, debug_out);
	
	//initialise file reader
	//todo fix this as well from above -> nullptr issues
	fReader = std::make_unique<FileReader>(roms_path, boot_rom_path);
	
	if (!fReader->is_initialied()) {
		m_core_context.initialised = false;
		return;
	}

	//set pointers for cpu 
	m_cpu.set_bus_ptr(&m_bus);
	m_cpu.set_timing_ptr(&m_timing);

	//attach components to the timing manager :: todo this will sync the emulator to the audio buffer eventually
	//gather up cycles for a second of audio then play the audio while gathering the next second
	m_timing.attach_components(&m_cpu);

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
bool SharpboyCore::emu_init(std::string rom_file_name) {
	if (!m_core_context.emu_ready) {
		Logger::log(log_error, "Instance not ready! Did you call cleanup?");
		return false;
	}

	return true;
}

//CALL WHEN CLOSING AN EMULATOR INSTANCE
void SharpboyCore::cleanup() {
	//reset components and destroy cart object
	m_cartridge.reset();
	m_bus.update_cartridge_ptr(nullptr);
	m_cpu.reset();

	m_core_context.emu_ready = true;
	Logger::log(log_status, "Cleanup successful, ready for new instance");
}

//EXECUTE ALL SINGLE STEP TESTS FOR NORMAL AND PREFIXED OPCODES (HALT, STOP, ILLEGAL NOT INCL.) PROOF OF CONCEPT REALLY
//THIS FUNCTION IS SLOW IN DEBUG MODE (30S +) RELEASE MODE (LESS THAN 5 SECONDS)
void SharpboyCore::run_ssts(std::string sst_path, bool show_all_results, bool prefixed) {
	SST sst(sst_path, 0x00, prefixed);

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

//DEBUG 
s_core_context* SharpboyCore::get_core_context() {
	return &m_core_context;
}