#pragma once

#include "Utilities/Common.h"
#include <vector>

//gb cartridge base class and single step test class, inherit from this for different mbc types

enum e_cart_types {
	CART_NONE,
	CART_SST,
	CART_ROM,
	CART_MBC1,
	CART_MBC2,
	CART_MBC3,
	CART_MBC4,
	CART_MBC5
};

struct s_cart_context {
	e_cart_types type = CART_NONE;
	std::string cart_title = "";
};

class Cartridge {
public:
	virtual ~Cartridge() = default;

	//rom loading functions
	virtual bool load_rom(std::string rom_file_name) = 0;

	//memory access functions
	virtual u8 read(u16 address) const = 0;
	virtual void write(u16 address, u8 value) = 0;

protected:
	//member variables
	std::vector<u8> m_rom;
	s_cart_context m_cartridge;
};