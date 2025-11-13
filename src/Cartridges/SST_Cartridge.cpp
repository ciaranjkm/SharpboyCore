#include "../include/Cartridges/SST_Cartridge.h"

CartridgeSST::CartridgeSST() {
	m_cartridge.type = CART_SST;
	m_cartridge.cart_title = "SST Cartridge";
}

CartridgeSST::~CartridgeSST() {
	m_rom.clear();
}

bool CartridgeSST::load_rom(std::string rom_file_name) {
	return false;
}

u8 CartridgeSST::read(u16 address) const {
	if (address >= 0x0000 && address <= 0xffff) {
		return m_rom[address];
	}

	return 0xFF;
}

void CartridgeSST::write(u16 address, u8 value) {
	m_rom[address] = value;
	return;
}

void CartridgeSST::sst_reset() {
	m_rom.clear();
	m_rom.resize(0x10000);
}

void CartridgeSST::unblocked_write(u16 address, u8 value) {
	if (address >= 0x0000 && address <= 0xffff) {
		m_rom[address] = value;
	}
}