#include <Components/Cartridges/CartMBC1.h>

CartMBC1::CartMBC1(e_cart_types type) {
	m_cartridge.type = type;
}

bool CartMBC1::load_rom(const std::vector<u8>& rom) {
	u8 rom_size_byte = rom[0x148];
	u8 ram_size_byte = rom[0x149];

	set_rom_ram_sizes(rom_size_byte, ram_size_byte);

	for (int i = 0; i < rom_size; i++) {
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
	if (address < 0x4000) {
		if (m_mode_register == 0x00) {
			return m_rom[address];
		}
		else {
			u32 target_bank = (m_ram_bank_register << 5);
			u32 target_address = (target_bank << 14) | (address);
			target_address %= rom_size;

			return m_rom[target_address];
		}
	}
	else if (address < 0x8000) {
		u32 target = ((m_rom_bank_register * 0x4000) + (address - 0x4000)) % rom_size;

		return m_rom[target];
	}
	else if (address >= 0xa000 && address < 0xc000)
	{
		if (!m_ram_enabled || !m_ram_global_enable) {
			return 0xff;
		}
	}
}

void CartMBC1::write(u16 address, u8 value) {
	//BANK REGISTER FOR THE BOOT ROM
	if (address == 0xff50) {
		m_bank_register = value;
		if (value == 0x01 && m_cartridge.boot_rom) {
			m_cartridge.boot_rom = false;
			swap_boot_rom_buffer();
			return;
		}
	}
	else if (address < 0x2000) {
		// RAM Enable
		m_ram_enable_register = value;
		m_ram_enabled = (value & 0x0F) == 0x0A;
		return;
	}
	else if (address < 0x4000) {
		// ROM Bank Number - lower 5 bits
		m_rom_bank_register = value & 0x1F;
		if ((m_rom_bank_register % 0x10) == 0x00) {
			m_rom_bank_register++;
		}
		return;
	}
	else if (address < 0x6000) {
		// RAM Bank Number / Upper ROM Bank bits
		m_ram_bank_register = value & 0x03;
		return;
	}
	else if (address < 0x8000) {
		// Banking Mode Select
		m_mode_register = value & 0x01;
		return;
	}
	else if (address >= 0xA000 && address < 0xC000) {
		// External RAM Write
		if (!m_ram_enabled || !m_ram_global_enable) {
			return;
		}

		u8 ram_bank = (m_mode_register == 0x01) ? m_ram_bank_register : 0x00;
		ram_bank &= (number_of_ram_banks - 1);

		int offset = ram_bank * 0x2000;
		u16 target_address = address - 0xA000;
		m_ram[offset + target_address] = value;
		return;
	}
}

//MEMBER FUNCTIONS
void CartMBC1::set_rom_ram_sizes(u8 rom_size, u8 ram_size) {
	//ROM SIZE FIRST (MAX 2MB)
	m_large_rom_mode = false;
	int resize_rom = 0;
	switch (rom_size) {
	case rom_32KB:
		resize_rom = 0x8000;
		break;

	case rom_64KB:
		resize_rom = 0x10000;
		break;

	case rom_128KB:
		resize_rom = 0x20000;
		break;

	case rom_256KB:
		resize_rom = 0x40000;
		break;

	case rom_512KB:
		resize_rom = 0x80000;
		break;

	case rom_1MB:
		resize_rom = 0x100000;
		m_large_rom_mode = true;
		break;

	case rom_2MB:
		resize_rom = 0x200000;
		m_large_rom_mode = true;
		break;

	default:
		resize_rom = 0x8000;
		m_rom_size = rom_32KB;
		break;
	}
	m_rom_size = (e_rom_size)rom_size;

	//RAM SIZE NEXT (MAX 32KB)
	int resize_ram = 0;
	switch (ram_size) {
	case ram_none:
		m_ram_enabled = false;
		resize_ram = 0x00;
		break;

	case ram_unused:
		m_ram_enabled = false;
		resize_ram = 0x00;
		break;

	case ram_8KB:
		m_ram_enabled = true;
		resize_ram = 0x2000;
		break;

	case ram_32KB:
		m_ram_enabled = true;
		if (m_large_rom_mode) {
			resize_ram = 0x2000;
			m_ram_size = ram_8KB;
		}
		else {
			resize_ram = 0x8000;
		}
		break;

	default:
		m_ram_enabled = false;
		resize_ram = 0;
		m_ram_size = ram_invalid;
		break;
	}
	m_ram_size = (e_ram_size)ram_size;

	m_rom.resize(resize_rom);
	m_ram.resize(resize_ram);

	number_of_rom_banks = resize_rom / 0x4000;
	number_of_ram_banks = resize_ram / 0x2000;

	this->rom_size = resize_rom;
	this->ram_size = resize_ram;
}