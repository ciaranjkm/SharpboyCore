#include "../include/SharpboyCore.h"

//INITIALISE FILEREADER AND OTHER DEPS
SharpboyCore::SharpboyCore(std::string roms_path, std::string boot_rom_path) {
	//initialise file reader
	fReader = std::make_unique<FileReader>(roms_path, boot_rom_path);
	
	if (!fReader->is_initialied()) {
		m_core.initialised = false;
		return;
	}

	//set pointers for cpu 
	m_cpu.set_bus_ptr(&m_bus);
	m_cpu.set_timing_ptr(&m_timing);

	//attach components to the timing manager
	m_timing.attach_components(&m_cpu);

	//init and ready to start new emu instance
	m_core.initialised = true;
	m_core.emu_ready = true;

	msg_status("Core created successfully");
}

//DESTROY POINTERS AND CLEANUP
SharpboyCore::~SharpboyCore() {
	//delete cartridge object
	m_cartridge.reset();

	msg_status("Core destroyed successfully");
}

//GETTER
bool SharpboyCore::is_initialised() const {
	return m_core.initialised;
}

//INITIALISE EMULATOR FOR SST RUN, REINITIALISE AFTER SST FINISH FOR A NEW INSTANCE
//CLEANUP NEEDS TO BE CALLED PRIOR TO INITIALISING A NEW INSTANCE
bool SharpboyCore::emu_init_for_sst() {
	if (!m_core.emu_ready) {
		msg_error("Instance not ready! Did you call cleanup?");
		return false;
	}

	//create new sst cartridge
	m_cartridge.reset();
	m_cartridge = std::make_unique<CartridgeSST>();
	
	//check for nullptr as will cast in single step test to load test data
	if (!m_cartridge) {
		return false;
	}

	//set new pointer in bus
	m_bus.set_cart_ptr(m_cartridge.get());
	m_bus.set_sst_mode();

	msg_status("SST initialisation successful");
	return true;
}

//INITIALISATION FOR NORMAL SHARPBOY RUN, REINITIALISE FOR A NEW INSTANCE,
//CLEANUP NEEDS TO BE CALLED PRIOR TO INITIALISING A NEW INSTANCE
bool SharpboyCore::emu_init(std::string rom_file_name) {
	if (!m_core.emu_ready) {
		msg_error("Instance not ready! Did you call cleanup?");
		return false;
	}

	return true;
}

//CALL WHEN CLOSING AN EMULATOR INSTANCE
void SharpboyCore::cleanup() {
	m_cartridge.reset();

	m_bus.set_cart_ptr(nullptr);
	m_bus.reset_sst_mode();

	m_cpu.reset_sst();

	m_core.emu_ready = true;
	msg_status("Cleanup successful, ready for new instance");
}

//execution
//EXECUTE ALL SINGLE STEP TESTS FOR NORMAL AND PREFIXED OPCODES
//HALT, STOP AND ILLGEAL OPCODES ARE NOT INCLUDED
void SharpboyCore::run_ssts(std::string sst_path, bool background_thread) {
	//cast to sst cart type to load test data
	CartridgeSST* sst_cart = dynamic_cast<CartridgeSST*>(m_cartridge.get());
	if (!sst_cart) {
		msg_error("Unable to cast to SST cartridge type, exiting...");
		return;
	}
	else {
		msg_status("SST cast successful\nStarting SSTs...");
	}

	//create sst object and init
	SST_Tester sst_tester = SST_Tester();
	sst_tester.init(sst_path, sst_cart, &m_cpu);
	if (!sst_tester.is_initialised()) {
		msg_status("SST object initialistion failed");
		return;
	}
	msg_status("SST object initialistion successful");

	//set a ptr to set for cpu to add cycles
	m_cpu.set_sst_ptr(&sst_tester);

	//test unprefixed opcodes
	for (int i = 0; i < SST_TEST_COUNT_NORMAL; i++) {
		if (!sst_tester.run_test(false, i)) {
			break;
		}
	}

	//display results 
	std::vector<s_test_result>* result = sst_tester.get_results();
	for (s_test_result r : *result) {
		std::cout << r.message << "\n";
	}

	//reset cart ptr
	sst_cart = nullptr;
	result = nullptr;
}

//DEBUG 
s_core_context* SharpboyCore::get_core_context() {
	return &m_core;
}