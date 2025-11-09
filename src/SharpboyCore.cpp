#include "../include/SharpboyCore.h"

SharpboyCore::SharpboyCore(std::string roms_path, std::string boot_rom_path) {
	fReader = std::make_unique<FileReader>(roms_path, boot_rom_path);
	
	if (!fReader->is_initialied()) {
		m_core.initialised = false;
		return;
	}

	m_core.initialised = true;
}

SharpboyCore::~SharpboyCore() {
	//delete cartridge object
	m_cartridge.reset();
}

//intialisation
bool SharpboyCore::is_initialised() const {
	return m_core.initialised;
}

bool SharpboyCore::emu_init_for_sst() {
	//reset all components here
	//check for init 

	//reset cartridge and make sst type
	m_cartridge.reset();
	m_cartridge = std::make_unique<CartridgeSST>();
	
	//check for nullptr as will cast in single step test to load test data
	if (!m_cartridge) {
		return false;
	}

	return true;
}

//execution
//EXECUTE ALL SINGLE STEP TESTS FOR NORMAL AND PREFIXED OPCODES
//HALT, STOP AND ILLGEAL OPCODES ARE NOT INCLUDED
void SharpboyCore::run_ssts(std::string sst_path) {
	CartridgeSST* sst_cart = dynamic_cast<CartridgeSST*>(m_cartridge.get());
	if (!sst_cart) {
		std::cout << "UNABLE TO CAST TO SST CART RETURNING...\n";
		return;
	}
	else {
		std::cout << "SST CAST SUCCESSFUL\nSTARTING SSTs...\n";
	}

	//read json file with sst data
	//reset cpu and cart

	//step cpu 

	//compare results

	//if bad log and continue

	//free ptr to cartridge
	sst_cart = nullptr;
}

//debug getters and setters
s_core_context* SharpboyCore::get_core_context() {
	return &m_core;
}