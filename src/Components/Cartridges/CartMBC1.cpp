#include <Components/Cartridges/CartMBC1.h>

CartMBC1::CartMBC1(e_cart_types type) {
	m_cartridge.type = type;
}

bool CartMBC1::load_rom(const std::vector<u8>& rom) {
	u8 rom_size = rom[0x148];
	u8 ram_size = rom[0x149];

	set_rom_ram_sizes(rom_size, ram_size);
	m_rom.resize(m_rom_size);
	m_ram.resize(m_ram_size);

	if (rom.size() < m_rom_size) {
		return false;
	}

	for (int i = 0; i < m_rom_size; i++) {
		m_rom[i] = rom[i];
	}

	return true;
}

bool CartMBC1::load_boot_rom(const std::vector<u8>& rom) {
	if (rom.size() != boot_rom_size) {
		return false;
	}

	for (int i = 0; i < boot_rom_size; i++) {
		m_boot_rom[i] = rom[i];
	}

	m_cartridge.boot_rom = true;
	return true;
}

void CartMBC1::swap_boot_rom_buffer() {
	std::array<u8, boot_rom_size> buffer = std::array<u8, boot_rom_size>();

	//SWAP FIRST 256 BYTES OF BOOT ROM AND ROM
	for (int i = 0; i < boot_rom_size; i++) {
		buffer[i] = m_rom[i];
		m_rom[i] = m_boot_rom[i];
		m_boot_rom[i] = buffer[i];
	}
}

u8 CartMBC1::read(u16 address) const {
	//ROM READS
	if (address < 0x4000) {
		if (m_simple_addressing_mode) {
			return m_rom[address];
		}
		else {
			//TODO ROM BANK 0 BANKING, NOT ENABLED ON SMALL ROMS SO JUST LEAVE FOR NOW
		}
	}
	else if (address < 0x8000) {
		u16 target_address = (u16)(((0x4000 * m_rom_bank_number) + (address - 0x4000)) % m_rom_size);
		return m_rom[target_address];
	}

	//RAM READS
	else if (address >= 0xa000 && address < 0xc000) {
		if (!m_local_ram_enabled) {
			return 0xff;
		}

		u16 target_address = ((0x2000 * m_ram_bank_number) + (address - 0x2000)) % m_ram_size;
		return m_ram[target_address];
	}

	return 0xff;
}

void CartMBC1::write(u16 address, u8 value) {
	if (address == 0xFF50) {
		m_bank_register = value;
		if (value == 0x01 && m_cartridge.boot_rom) {
			m_cartridge.boot_rom = false;
			swap_boot_rom_buffer();
		}
		return;
	}

	if (address < 0x2000) {
		//CAN RAM BE ACCESSED, NOT IS RAM ENABLED FOR THIS CART
		m_local_ram_enabled = (value & 0x0f) == 0x0a;
	}
	else if (address < 0x4000) {
		//WRTITES TO ROM BANK REGISTERS, 0x00 BECOMES 0x01
		u8 bank = (value == 0x00) ? 1 : value;
		m_rom_bank_number = (bank & 0x60) | (value & 0x1f);
	}
	else if (address < 0x6000) {
		//WRITE TO RAM BANK REGISTER OR IN LARGE ROM MODE, WRITES TO BIT 5/6 OF ROM BANK REGISTER
		if (!m_large_rom_mode) {
			m_ram_bank_number = value & 0x03;
		}
		else {
			m_rom_bank_number = (m_rom_bank_number & 0x1f) | ((value & 0x03) << 5);
		}
	}
	else if (address < 0x8000) {
		//WRITE TO TOGGLE ADDRESSING MODE, THIS HAS NO EFFECT IF RAM <= 8KB and ROM <= 512KB (NOT LARGE ROM MODE)
		if (!(m_rom_size >= 0x80000 && m_ram_size >= 0x2000)) {
			return;
		}

		if ((value & 0x01) != 0x00) {
			m_simple_addressing_mode = false;
		}
		else {
			m_simple_addressing_mode = true;
		}
	}
}

//MEMBER FUNCTIONS
void CartMBC1::set_rom_ram_sizes(u8 rom_size, u8 ram_size) {
	//ROM SIZE FIRST (MAX 2MB)
	m_large_rom_mode = false;
	switch (rom_size) {
	case rom_32KB:
		m_rom_size = 0x8000;
		break;

	case rom_64KB:
		m_rom_size = 0x10000;
		break;

	case rom_128KB:
		m_rom_size = 0x20000;
		break;

	case rom_256KB:
		m_rom_size = 0x40000;
		break;

	case rom_512KB:
		m_rom_size = 0x80000;
		break;

	case rom_1MB:
		m_rom_size = 0x100000;
		m_large_rom_mode = true;
		break;

	case rom_2MB:
		m_rom_size = 0x200000;
		m_large_rom_mode = true;
		break;

	default:
		m_rom_size = 0x8000;
		m_rom_size_type = rom_32KB;
		break;
	}
	m_rom_size_type = (e_rom_size)rom_size;

	//RAM SIZE NEXT (MAX 32KB)
	switch (ram_size) {
	case ram_none:
		m_ram_enabled = false;
		m_ram_size = 0x00;
		break;

	case ram_unused:
		m_ram_enabled = false;
		m_ram_size = 0x00;
		break;

	case ram_8KB:
		m_ram_enabled = true;
		m_ram_size = 0x2000;
		break;

	case ram_32KB:
		m_ram_enabled = true;
		if (m_large_rom_mode) {
			m_ram_size = 0x2000;
			m_ram_size_type = ram_8KB;
		}
		else {
			m_ram_size = 0x8000;
		}
		break;

	default:
		m_ram_enabled = false;
		m_ram_size = 0x00;
		m_ram_size_type = ram_invalid;
		break;
	}
	m_ram_size_type = (e_ram_size)ram_size;
}