#pragma once
#include "../../Common.h"
#include <vector>

enum e_cart_types {
	CART_NONE = -1,
	CART_SST = -2,
	CART_ROM = 0x00,
	CART_ROM_RAM = 0x08,
	CART_ROM_RAM_BATTERY = 0x09,
	CART_MBC1 = 0x01,
	CART_MBC1_RAM = 0x02,
	CART_MBC1_RAM_BATTERY = 0x03,
	CART_MBC2 = 0x05,
	CART_MBC3 = 0x11,
	CART_MBC5 = 0x19,
};

enum e_rom_size {
	rom_32KB = 0x00,
	rom_64KB = 0x01,
	rom_128KB = 0x02,
	rom_256KB = 0x03,
	rom_512KB = 0x04,
	rom_1MB = 0x05,
	rom_2MB = 0x06,
	rom_4MB = 0x07,
	rom_8MB = 0x08,
	rom_invalid = 0xff,
};

enum e_ram_size {
	ram_none = 0x00,
	ram_unused = 0x01,
	ram_8KB = 0x02,
	ram_32KB = 0x03,
	ram_128KB = 0x04,
	ram_64KB = 0x05,
	ram_invalid = 0xff
};

struct s_cart_context {
	e_cart_types type = CART_NONE;
	std::string cart_title = "";

	bool boot_rom = false;
};

const int boot_rom_size = 0x100;

class Cartridge {
public:
	virtual ~Cartridge() = default;

	//ROM LOADING
	virtual bool load_rom(const std::vector<u8>& rom) = 0;
	virtual bool load_boot_rom(const std::vector<u8>& boot_rom) = 0;

	virtual void swap_boot_rom_buffer() = 0;

	//MEMORY ACCESS
	virtual u8 read(u16 address) const = 0;
	virtual void write(u16 address, u8 value) = 0;

protected:
	s_cart_context m_cartridge;
	u8 m_bank_register = 0x00;
};