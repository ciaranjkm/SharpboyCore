#pragma once

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

#include "Utilities/FileReader.h"
#include "Utilities/SST.h"
#include "Utilities/Logger.h"

#include "Sync.h"
#include "ComponentManager.h"

struct s_core_context {
	bool initialised = false;

	bool emu_ready = false;
	bool emu_active = false;
	bool using_boot_rom = false;
};

//TODO REFACTOR ERROR AND LOGGING
enum e_error_type {
	error_fatal,
	error_initialisation,
	error_other,
	error_none
};

struct s_core_error {
	e_error_type type = error_none;
	std::string current_error = "";

	s_core_error(e_error_type t = error_none, std::string e = "") {
		type = t;
		current_error = e;
	}

	std::string get_error() const {
		return current_error;
	}

	void output() {
		Logger::log(log_error, current_error);
	}
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

	//RUN TO BE CALLED IN THE MAIN LOOP
	void run();

	//JOYPAD
	void set_new_joypad_state(s_joypad_state state);
	s_joypad_state* get_joypad_state();

	//DISPLAY
	void reset_frame_ready();
	bool get_frame_ready();

	//AUDIO
	void reset_audio_buffer_ready();
	bool get_audio_buffer_ready();

	//DEBUG + SST
	/*
	EXECUTE ALL SINGLE STEP TESTS FOR NORMAL AND PREFIXED OPCODES (HALT, STOP, ILLEGAL NOT INCL.) PROOF OF CONCEPT REALLY
	USES THE SST PATH IN FILEREADER STATIC OBJECT, MAKE SURE IT IS UPDATED BEFORE RUNNING OR ALL FAILS
	*/
	void run_ssts(bool show_all_results, bool prefixed);

	std::string get_error() const;
	void output_current_error();

	s_core_context* get_core_context();

private:
	//CONTEXTS
	s_core_context m_core_context;
	s_core_error m_current_error;

	//COMPONENTS
	ComponentManager m_components;
	Syncroniser m_syncroniser;

	//SINGLE STEP TEST 
	s_core_sst_context m_sst_context;

};