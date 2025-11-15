#pragma once

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

#include "Utilities/FileReader.h"
#include "Utilities/SST.h"
#include "Utilities/Logger.h"

#include "CPU/CPU.h"
#include "Bus.h"
#include "Cartridges/CartDefs.h"
#include "Timing.h"

struct s_core_context {
	bool initialised = false;
	bool emu_ready = false;
	bool is_running = false;
	bool using_boot_rom = false;
};

struct s_core_sst_context {
	std::atomic_bool sst_active = false;
};

/*
	This is the main class used to create an emulator instance everything is handled for you here in the public api.
	Give paths to Sharpboy on construction or use FileReader static object to update the paths -> updates FileReader in const
*/

class SharpboyCore {
public:
	//CONS | DEST
	SharpboyCore(std::string roms_path, std::string boot_rom_path, std::string sst_path = "", bool status_out = true, bool debug_out = false);
	~SharpboyCore();

	//RETURN IS SHARPBOY INITIALISED SUCCESSFULLY
	bool is_initialised() const;

	//START AND CLEANUP AN EMULATOR INSTANCE
	bool emu_init(std::string rom_file_name, bool using_boot_rom);
	void cleanup();

	//RUN TO BE CALLED IN THE MAIN LOOP
	void run();

	//DEBUG + SST
	void run_ssts(bool show_all_results, bool prefixed);
	s_core_context* get_core_context();

private:
	//CONTEXT STRUCT
	s_core_context m_core_context;
	s_core_sst_context m_sst_context;

	//EMU COMPONENTS
	std::unique_ptr<Cartridge> m_cartridge = nullptr;
	std::unique_ptr<IMU> m_imu = nullptr;
	CPU m_cpu;
	Bus m_bus;

	//TIMING FOR THE SYSTEM
	Timing m_timing;

private:
	//MEMBER FUNCTIONS
	bool read_rom_file(std::vector<u8>& rom, std::string file_name, bool boot_rom);
	bool create_new_mbc(const e_cart_types cart_type, u8 rom_size, u8 ram_size);
};