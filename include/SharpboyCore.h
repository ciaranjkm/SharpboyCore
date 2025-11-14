#pragma once

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

#include "Utilities/FileReader.h"
#include "Utilities/Testing.h"
#include "Utilities/Logger.h"

#include "CPU.h"
#include "Bus.h"
#include "Cartridges/CartDefs.h"
#include "Timing.h"

struct s_core_context {
	bool initialised = false;
	bool emu_ready = false;
	bool is_running = false;
};

struct s_core_sst_context {
	std::atomic_bool sst_active = false;
};

class SharpboyCore {
public:
	//CONS | DEST
	SharpboyCore(std::string roms_path, std::string boot_rom_path, bool status_out = true, bool debug_out = false);
	~SharpboyCore();

	//INIT SHUDOWN
	bool is_initialised() const;
	void cleanup();

	bool emu_init(std::string rom_file_name);

	//RUN
	void run_ssts(std::string sst_path, bool background_thread);
	void run();

	//DEBUG
	s_core_context* get_core_context();

private:
	//MEMBER VARIABLES

	//CONTEXT STRUCT
	s_core_context m_core_context;
	s_core_sst_context m_sst_context;

	//EMU COMPONENTS
	std::unique_ptr<Cartridge> m_cartridge;
	CPU m_cpu;
	Bus m_bus;

	//TIMING FOR THE SYSTEM
	Timing m_timing;
};