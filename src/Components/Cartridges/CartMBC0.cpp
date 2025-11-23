#include <Components/Cartridges/CartMBC0.h>

CartMBC0::CartMBC0(e_cart_types type, bool ram_enabled, bool battery_enabled) {
	m_ram_enabled = ram_enabled;
	m_battery_enabled = battery_enabled;
	m_cartridge.type = type;
}

bool CartMBC0::load_rom(const std::vector<u8>& rom) {
	//DONT CHECK SIZE ONLY 32KB ALLOWED WITH NO BANKING
	//ROM CANNOT BE ACCESSED SO DONT COPY IT INTO ROM
	size_t size = (rom.size() > rom_size ? rom_size : rom.size());
	for (int i = 0; i < size; i++) {
		m_rom[i] = rom[i];
	}

	return true;
}

bool CartMBC0::load_boot_rom(const std::vector<u8>& rom) {
	if (rom.size() != boot_rom_size) {
		return false;
	}

	for (int i = 0; i < boot_rom_size; i++) {
		m_boot_rom[i] = rom[i];
	}

	m_cartridge.boot_rom = true;
	return true;
}

void CartMBC0::swap_boot_rom_buffer() {
	std::array<u8, boot_rom_size> buffer = std::array<u8, boot_rom_size>();
	
	//SWAP FIRST 256 BYTES OF BOOT ROM AND ROM
	for (int i = 0; i < boot_rom_size; i++) {
		buffer[i] = m_rom[i];
		m_rom[i] = m_boot_rom[i];
		m_boot_rom[i] = buffer[i];
	}
}

u8 CartMBC0::read(u16 address) const {
	if (address == 0xff50) {
		return m_bank_register;
	}
	else if (address >= 0x0000 && address < 0x8000) {
		return m_rom[address];
	}
	else if (address >= 0xa000 && address < 0xc000) {
		if (!m_ram_enabled) {
			return 0xff;
		}

		return m_ram[(u16)(address - 0xa000)];
	}
	else {
		return 0xff;
	}
}

void CartMBC0::write(u16 address, u8 value) {
	if (address == 0xff50) {
		m_bank_register = value;

		if (value == 0x01 && m_cartridge.boot_rom) {
			m_cartridge.boot_rom = false;
			swap_boot_rom_buffer();
		}
	}
	else if (address >= 0x0000 && address < 0x8000) {
		//cannot write to cartridge rom
		return;
	}
	else if (address >= 0xa000 && address < 0xc000) {
		if (!m_ram_enabled) {
			return;
		}

		m_ram[(u16)(address - 0xa000)] = value;
	}
}