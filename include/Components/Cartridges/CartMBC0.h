#pragma once
#include "Cartridge.h"
#include "../../Utilities/FileReader.h"

#include <array>

const int rom_size = 0x8000;
const int ram_size = 0x2000;

class CartMBC0 final : public Cartridge  {
public:
	CartMBC0(e_cart_types type = CART_ROM, bool ram_enabled = false, bool battery_enabled = false);

	bool load_rom(const std::vector<u8>& rom) override;
	bool load_boot_rom(const std::vector<u8>& boot_rom) override;

	void swap_boot_rom_buffer() override;

	//MEMORY ACCESS
	u8 read(u16 address) const override;
	void write(u16 address, u8 value) override;
private:
	//MEMBER VARIABLES
	std::array<u8, rom_size> m_rom = std::array<u8, rom_size>();
	std::array<u8, boot_rom_size> m_boot_rom = std::array<u8, boot_rom_size>();
	std::array<u8, ram_size> m_ram = std::array<u8, ram_size>();

	bool m_ram_enabled = false;
	bool m_battery_enabled = false;
};