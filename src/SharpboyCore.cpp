#include "../include/SharpboyCore.h"

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

	//attach components to the timing manager
	m_timing.attach_components(&m_cpu);

	//init and ready to start new emu instance
	m_core_context.initialised = true;
	m_core_context.emu_ready = true;

	Logger::log(log_default, "SharpboyCore created successfully]");
}

//DESTROY POINTERS AND CLEANUP
SharpboyCore::~SharpboyCore() {
	//delete cartridge object
	m_cartridge.reset();
	Logger::log(log_default, "Core destroyed successfully");
}

//GETTER
bool SharpboyCore::is_initialised() const {
	return m_core_context.initialised;
}

//INITIALISATION FOR NORMAL SHARPBOY RUN, REINITIALISE FOR A NEW INSTANCE,
//CLEANUP NEEDS TO BE CALLED PRIOR TO INITIALISING A NEW INSTANCE
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
	m_bus.reset_sst_mode();
	m_cpu.reset();

	m_core_context.emu_ready = true;
	Logger::log(log_status, "Cleanup successful, ready for new instance");
}

//execution
//EXECUTE ALL SINGLE STEP TESTS FOR NORMAL AND PREFIXED OPCODES
//HALT, STOP AND ILLGEAL OPCODES ARE NOT INCLUDED

//CAN BE RUN AS BACKGROUND THREAD WITH FLAG SET
void SharpboyCore::run_ssts(std::string sst_path, bool background_thread) {
	SST sst(sst_path, 0x00, false);

	bool test_complete = false;
	std::thread sst_thread = std::thread([&sst]() {sst.run(); });
	sst_thread.detach();

	while (!sst.is_test_complete()) {
		Logger::log(log_sst_status, "completing ssts...");
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}

	if (sst_thread.joinable()) {
		sst_thread.join();
	}

	std::array<s_test_result, SMALL_TEST_COUNT>* results = sst.get_results();
	for (s_test_result result : *results) {
		std::cout << result.msg << "\n";
	}

	results = nullptr;
}

//DEBUG 
s_core_context* SharpboyCore::get_core_context() {
	return &m_core_context;
}