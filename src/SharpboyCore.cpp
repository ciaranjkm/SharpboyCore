#include "../include/SharpboyCore.h"

//INITIALISE FILEREADER AND OTHER DEPS
SharpboyCore::SharpboyCore(std::string roms_path, std::string boot_rom_path, bool status_out, bool debug_out) {
	update_logger(status_out, debug_out);
	
	//initialise file reader
	fReader = std::make_unique<FileReader>(roms_path, boot_rom_path);
	
	if (!fReader->is_initialied()) {
		m_core_context.initialised = false;
		return;
	}

	//set pointers for cpu 
	m_cpu.set_bus_ptr(&m_bus);
	m_cpu.set_timing_ptr(&m_timing);

	//attach components to the timing manager
	m_timing.attach_components(&m_cpu);

	//init and ready to start new emu instance
	m_core_context.initialised = true;
	m_core_context.emu_ready = true;

	msg_default("SharpboyCore created successfully");
}

//DESTROY POINTERS AND CLEANUP
SharpboyCore::~SharpboyCore() {
	//delete cartridge object
	m_cartridge.reset();

	msg_status("Core destroyed successfully");
}

//GETTER
bool SharpboyCore::is_initialised() const {
	return m_core_context.initialised;
}

//INITIALISE EMULATOR FOR SST RUN, REINITIALISE AFTER SST FINISH FOR A NEW INSTANCE
//CLEANUP NEEDS TO BE CALLED PRIOR TO INITIALISING A NEW INSTANCE
bool SharpboyCore::emu_init_for_sst() {
	if (!m_core_context.emu_ready) {
		msg_error("Instance not ready! Did you call cleanup?");
		return false;
	}

	//create new sst cartridge and update bus pointer
	m_cartridge.reset();
	m_cartridge = std::make_unique<CartridgeSST>();
	m_bus.set_sst_mode(m_cartridge.get());

	msg_status("Initialised for SSTs");
	return true;
}

//INITIALISATION FOR NORMAL SHARPBOY RUN, REINITIALISE FOR A NEW INSTANCE,
//CLEANUP NEEDS TO BE CALLED PRIOR TO INITIALISING A NEW INSTANCE
bool SharpboyCore::emu_init(std::string rom_file_name) {
	if (!m_core_context.emu_ready) {
		msg_error("Instance not ready! Did you call cleanup?");
		return false;
	}

	return true;
}

//CALL WHEN CLOSING AN EMULATOR INSTANCE
void SharpboyCore::cleanup() {
	//reset components and destroy cart object
	m_cartridge.reset();
	m_bus.reset_sst_mode();
	m_cpu.reset_sst();

	m_core_context.emu_ready = true;
	msg_status("Cleanup successful, ready for new instance");
}

//execution
//EXECUTE ALL SINGLE STEP TESTS FOR NORMAL AND PREFIXED OPCODES
//HALT, STOP AND ILLGEAL OPCODES ARE NOT INCLUDED

//CAN BE RUN AS BACKGROUND THREAD WITH FLAG SET
void SharpboyCore::run_ssts(std::string sst_path, bool background_thread) {
	SST_Tester sst_tester = SST_Tester();
	sst_tester.init(sst_path, m_cartridge.get(), &m_cpu);
	if (!sst_tester.is_initialised()) {
		return;
	}

	//RUN THE TESTS FOR UNPREFIXED OPCODES IN A SEPERATE THREAD
	msg_status("SSTs running...");
	if (!background_thread) {
		int test_number = 0;
		
		while (test_number < SST_TEST_COUNT_NORMAL) {
			sst_tester.run_test(false, test_number);
			test_number++;

			msg_sst_progress(false, test_number, SST_TEST_COUNT_NORMAL);
		}	
	}
	else {
		//THIS FUNCTION DOESNT CLOSE THIS THREAD
		//HANDLE IN CLI OR GUI TO PICKUP AND CLOSE THREAD

		//WHEN CLOSING THE THREAD
		std::thread sst_thread = std::thread([&sst_tester]() {
			for (int i = 0; i < SST_TEST_COUNT_NORMAL; i++) {
				sst_tester.run_test(false, i);
			}
		});
	}
	
	m_sst_context.sst_active.store(false);
}

//DEBUG 
s_core_context* SharpboyCore::get_core_context() {
	return &m_core_context;
}