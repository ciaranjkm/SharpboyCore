#pragma once

#include "Common.h"
#include <vector>

//gb cartridge base class and single step test class, inherit from this for different mbc types

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
	std::vector<u8> rom_data;
};