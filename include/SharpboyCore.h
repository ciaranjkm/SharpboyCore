#pragma once

#include <iostream>
#include <thread>
#include <chrono>

#include "Utilities/FileReader.h"
#include "Utilities/SST_Defs.h"
#include "Utilities/NlohmannJSON/json.hpp"

//components
#include "Cartridges/CartDefs.h"

struct s_core_context {
	bool initialised = false;
	bool emu_ready = false;
	bool is_running = false;
};

class SharpboyCore {
public:
	SharpboyCore(std::string roms_path, std::string boot_rom_path);
	~SharpboyCore();

	//initialisation
	bool is_initialised() const;
	void cleanup();

	bool emu_init_for_sst();
	bool emu_init(std::string rom_file_name);

	//execution
	void run_ssts(std::string sst_path, bool background_thread);
	void run();

	//debug getters/setters
	s_core_context* get_core_context();

private:
	//member variables
	s_core_context m_core;

	//emulation components
	std::unique_ptr<Cartridge> m_cartridge;
};