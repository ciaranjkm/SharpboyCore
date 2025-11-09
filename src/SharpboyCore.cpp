#include "../include/SharpboyCore.h"

//INITIALISE FILEREADER AND OTHER DEPS
SharpboyCore::SharpboyCore(std::string roms_path, std::string boot_rom_path) {
	fReader = std::make_unique<FileReader>(roms_path, boot_rom_path);
	
	if (!fReader->is_initialied()) {
		m_core.initialised = false;
		return;
	}

	m_core.initialised = true;
	m_core.emu_ready = true;
}

//DESTROY POINTERS AND CLEANUP
SharpboyCore::~SharpboyCore() {
	//delete cartridge object
	m_cartridge.reset();
}

//GETTER
bool SharpboyCore::is_initialised() const {
	return m_core.initialised;
}

//INITIALISE EMULATOR FOR SST RUN, REINITIALISE AFTER SST FINISH FOR A NEW INSTANCE
//CLEANUP NEEDS TO BE CALLED PRIOR TO INITIALISING A NEW INSTANCE
bool SharpboyCore::emu_init_for_sst() {
	if (!m_core.emu_ready) {
		std::cout << "INSTANCE NOT READY! DID YOU CALL CLEANUP?\n";
		return false;
	}

	//reset cpu 

	//reset cartridge and make sst type
	m_cartridge.reset();
	m_cartridge = std::make_unique<CartridgeSST>();
	
	//check for nullptr as will cast in single step test to load test data
	if (!m_cartridge) {
		return false;
	}

	//reset bus to sst mode
	//add cart and cpu to bus

	return true;
}

//INITIALISATION FOR NORMAL SHARPBOY RUN, REINITIALISE FOR A NEW INSTANCE,
//CLEANUP NEEDS TO BE CALLED PRIOR TO INITIALISING A NEW INSTANCE
bool SharpboyCore::emu_init(std::string rom_file_name) {
	if (!m_core.emu_ready) {
		std::cout << "INSTANCE NOT READY! DID YOU CALL CLEANUP?\n";
		return false;
	}

	return true;
}

//CALL WHEN CLOSING AN EMULATOR INSTANCE
void SharpboyCore::cleanup() {
	m_cartridge.reset();

	m_core.emu_ready = true;
	std::cout << "CLEANUP SUCCESSFUL. READY FOR NEW INSTANCE\n";
}

//execution
//EXECUTE ALL SINGLE STEP TESTS FOR NORMAL AND PREFIXED OPCODES
//HALT, STOP AND ILLGEAL OPCODES ARE NOT INCLUDED
void SharpboyCore::run_ssts(std::string sst_path, bool background_thread) {
	//get a ptr for sst cart
	CartridgeSST* sst_cart = dynamic_cast<CartridgeSST*>(m_cartridge.get());
	if (!sst_cart) {
		std::cout << "UNABLE TO CAST TO SST CART RETURNING...\n";
		return;
	}
	else {
		std::cout << "SST CAST SUCCESSFUL\nSTARTING SSTs...\n";
	}

	//create sst object and init
	SST_Tester test = SST_Tester();
	test.init(sst_path, sst_cart);
	if (!test.is_initialised()) {
		std::cout << "FAILED SST INITIALISATION!\n";
		return;
	}

	//UNPREFIXED OPCODES
	for (int i = 0; i < SST_TEST_COUNT_NORMAL; i++) {
		if (!test.run_test(false, i)) {
			break;
		}
	}

	std::vector<s_test_result>* result = test.get_results();
	for (s_test_result r : *result) {
		std::cout << r.message << "\n";
	}

	//RESET CART PTR
	sst_cart = nullptr;
}

//DEBUG 
s_core_context* SharpboyCore::get_core_context() {
	return &m_core;
}