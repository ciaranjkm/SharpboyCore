#pragma once

#include <iostream>
#include "FileReader.h"

//components
#include "Cartridges/Cartridges.h"

struct s_core_context {
	bool initialised = false;
	bool is_running = false;
};

class SharpboyCore {
public:
	SharpboyCore(std::string roms_path, std::string boot_rom_path);
	~SharpboyCore();

	//initialisation
	bool is_initialised() const;

	bool emu_init_for_sst();
	bool emu_init(std::string rom_file_name);

	//execution
	void run_ssts(std::string sst_path);
	void run();

	//debug getters/setters
	s_core_context* get_core_context();

private:
	//member variables
	s_core_context m_core;

	//emulation components
	std::unique_ptr<Cartridge> m_cartridge;
};