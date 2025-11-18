#include <Components/Cartridges/Cartridge.h>

bool Cartridge::load_rom(const std::vector<u8>& rom) {
	return false;
}

bool Cartridge::load_boot_rom(const std::vector<u8>& boot_rom) {
	return false;
}

u8 Cartridge::read(u16 address) const {
	return 0xff;
}

void Cartridge::write(u16 address, u8 value) {
	return;
}