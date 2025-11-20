#pragma once

#include <array>

#include "Cartridge.h"
#include "../../Utilities/Logger.h"

typedef uint32_t u32;

/*
	In the default configuration supports up to 512KiB of ROM with up to 32KiB of banked RAM.
	
	Some carts use larger ROMs (< 1MiB) up to 2MiB, only supporting 8KiB of fixed RAM. All carts 
	with ROM larger than 1MiB use this alernative pattern.
*/

class CartMBC1 final : public Cartridge {
public:
	CartMBC1(e_cart_types type);

	//VIRTUAL FUNCTIONS
	bool load_rom(const std::vector<u8>& rom) override;
	bool load_boot_rom(const std::vector<u8>& rom) override;

	void swap_boot_rom_buffer() override;

	u8 read(u16 address) const override;
	void write(u16 address, u8 value) override;

private:
	//MEMBER VARIABLES
	std::vector<u8> m_rom = std::vector<u8>();
	std::vector<u8> m_ram = std::vector<u8>();
	std::array<u8, boot_rom_size> m_boot_rom = std::array<u8, boot_rom_size>();

	u8 m_rom_bank_number = 0x01;
	u8 m_ram_bank_number = 0x00;
	bool m_simple_addressing_mode = true;
	bool m_local_ram_enabled = false;

	bool m_large_rom_mode = false;
	bool m_ram_enabled = false;

	int m_rom_size = 0;
	e_rom_size m_rom_size_type = rom_invalid;

	int m_ram_size = 0;
	e_ram_size m_ram_size_type = ram_invalid;

private:
	//MEMBER FUNCTIONS
	void set_rom_ram_sizes(u8 rom_size, u8 ram_size);
};