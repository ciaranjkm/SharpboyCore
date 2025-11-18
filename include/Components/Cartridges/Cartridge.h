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
	CART_MBC2 = 0x05,
	CART_MBC3 = 0x11,
	CART_MBC5 = 0x19,
};

struct s_cart_context {
	e_cart_types type = CART_NONE;
	std::string cart_title = "";

	bool ram_enabled = false;
	bool battery_enabled = false;

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
	u8 m_bank_register = 0x00;
	s_cart_context m_cartridge;
};