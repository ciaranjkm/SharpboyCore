#pragma once

#include <iostream>
#include <thread>
#include <chrono>

#include "Utilities/FileReader.h"
#include "Utilities/Testing.h"

#include "Cartridges/CartDefs.h"

struct s_core_context {
	bool initialised = false;
	bool emu_ready = false;
	bool is_running = false;
};

class SharpboyCore {
public:
	//CONS | DEST
	SharpboyCore(std::string roms_path, std::string boot_rom_path);
	~SharpboyCore();

	//INIT SHUDOWN
	bool is_initialised() const;
	void cleanup();

	bool emu_init_for_sst();
	bool emu_init(std::string rom_file_name);

	//RUN
	void run_ssts(std::string sst_path, bool background_thread);
	void run();

	//DEBUG
	s_core_context* get_core_context();

private:
	//MEMBER VARIABLES
	s_core_context m_core;

	//EMU COMPONENTS
	std::unique_ptr<Cartridge> m_cartridge;
};