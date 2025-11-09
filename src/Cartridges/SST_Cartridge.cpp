#include "../include/Cartridges/SST_Cartridge.h"

bool CartridgeSST::load_rom(std::string rom_file_name) {
	return false;
}

u8 CartridgeSST::read(u16 address) const {
	if (address >= 0x0000 && address <= 0xffff) {
		return rom_data[address];
	}

	return 0xFF;
}

void CartridgeSST::write(u16 address, u8 value) {
	printf("CART WRITE: 0x%04X <- 0x%02X\n", address, value);
	return;
}