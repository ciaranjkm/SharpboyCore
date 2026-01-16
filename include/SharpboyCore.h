#pragma once

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

#include "Utilities/SBReader.h"
#include "Utilities/SBLogger.h"

#include "Sync.h"
#include "ComponentManager.h"

struct s_core_context {
	bool initialised = false;

	bool emu_ready = false;
	bool emu_active = false;
	bool using_boot_rom = false;

	std::filesystem::path roms_directory = "";
	std::filesystem::path boot_rom_file = "";
};

struct s_core_sst_context {
	std::atomic_bool sst_active = false;
};

/*
	This is the main class used to create an emulator instance everything is handled for you here in the public api.

	Give base directories for Sharpboy with static FileReader object or just leave it blank and use the path given to init
	Initialise a new instance and call run in your main loop, catching the display and audio buffer for output

	Examples on Github:
		TODO
		TODO
		TODO
*/

class SharpboyCore {
public:
	//CONS | DEST
	SharpboyCore();
	~SharpboyCore();

	bool is_initialised() const;

	//INSTANCE STARTUP CLEAUP
	bool initialise_new_instance(std::filesystem::path rom_file_name, bool using_boot_rom = false);
	void cleanup_current_instance();

	void set_roms_directory(std::filesystem::path roms_directory);
	std::filesystem::path get_roms_directory() const;

	void set_boot_rom_file(std::filesystem::path path);
	std::filesystem::path get_boot_rom_file() const;

	//RUN TO BE CALLED IN THE MAIN LOOP
	int run();

	//JOYPAD
	void set_new_joypad_state(s_joypad_state state);
	s_joypad_state* get_joypad_state();

	//DISPLAY
	void reset_frame_ready();
	bool get_frame_ready();
	std::array<u32, 160 * 144>* get_frame_buffer();

	//AUDIO
	void reset_audio_buffer_ready();
	bool get_audio_buffer_ready();

	//DEBUG
	s_core_context* get_core_context();
	ComponentManager* get_component_manager();

private:
	//CONTEXTS
	s_core_context m_core_context;

	//COMPONENTS
	ComponentManager m_components;
	Syncroniser m_syncroniser;
};