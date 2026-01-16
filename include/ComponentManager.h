#pragma once

#include "Sync.h"
#include "Components/CPU/CPU.h"
#include "Components/Bus.h"
#include "Components/IMU.h"
#include "Components/PPU.h"
#include "Components/Timer.h"
#include "Components/Cartridges/CartDefs.h"
#include "Components/Joypad.h"

#include <memory>

enum e_components {
	comp_cpu,
	comp_bus,
	comp_imu,
	comp_ppu,
	comp_timer,
	comp_cart
};

class Syncroniser;

class ComponentManager {
public:
	//COMPONENT LINKING
	bool link_components(Syncroniser* syncroniser);

	//NEW INSTANCE | RESET AND INITIALSE ALL COMPONENTS
	void initialise_components(bool using_boot_rom);
	bool assign_cart_type(e_cart_types cart_type);
	void load_rom_into_cart(const std::vector<u8>& rom, bool using_boot_rom, const std::vector<u8>& boot_rom);

	//RESET ALL COMPONENTS
	void reset_components();

	//COMPONENT GETTERS
	CPU* get_cpu();
	Bus* get_bus();	
	IMU* get_imu();
	PPU* get_ppu();
	Timer* get_timer();
	Cartridge* get_base_cart();

	//EXTRA GETTERS + SETTERS
	s_registers* get_cpu_registers();
	bool is_ppu_frame_ready();
	void ppu_reset_frame_ready();
	std::array<u32, 160 * 144>* get_ppu_frame_buffer();
	std::array<u8, VRAM_SIZE>* get_ppu_vram();
	void set_joypad_state(s_joypad_state state);

private:
	//MEMBER VARIABLES
	bool m_components_linked = false;

	//COMPONENT INSTANCES
	CPU m_cpu;
	Bus m_bus;
	IMU m_imu;
	PPU m_ppu;
	Timer m_timer;
	Joypad m_joypad;
	std::unique_ptr<Cartridge> m_cartridge = nullptr;
};