#include "../include/SharpboyCore.h"

SharpboyCore::SharpboyCore(std::string roms_path, std::string boot_rom_path) {
	fReader = std::make_unique<FileReader>(roms_path, boot_rom_path);
	
	if (!fReader->is_initialied()) {
		m_core.initialised = false;
		return;
	}

	m_core.initialised = true;
	m_core.emu_ready = true;
}

SharpboyCore::~SharpboyCore() {
	//delete cartridge object
	m_cartridge.reset();
}

bool SharpboyCore::is_initialised() const {
	return m_core.initialised;
}

//INITIALISE EMULATOR FOR SST RUN, REINITIALISE AFTER SST FINISH FOR A NEW INSTANCE
//CLEANUP NEEDS TO BE CALLED PRIOR TO INITIALISING A NEW INSTANCE
bool SharpboyCore::emu_init_for_sst() {
	if (!m_core.emu_ready) {
		std::cout << "INSTANCE NOT READY! DID YOU CALL CLEANUP?\n";
		return false;
	}

	//reset cpu 

	//reset cartridge and make sst type
	m_cartridge.reset();
	m_cartridge = std::make_unique<CartridgeSST>();
	
	//check for nullptr as will cast in single step test to load test data
	if (!m_cartridge) {
		return false;
	}

	//reset bus to sst mode
	//add cart and cpu to bus

	return true;
}

//INITIALISATION FOR NORMAL SHARPBOY RUN, REINITIALISE FOR A NEW INSTANCE,
//CLEANUP NEEDS TO BE CALLED PRIOR TO INITIALISING A NEW INSTANCE
bool SharpboyCore::emu_init(std::string rom_file_name) {
	if (!m_core.emu_ready) {
		std::cout << "INSTANCE NOT READY! DID YOU CALL CLEANUP?\n";
		return false;
	}


	return true;
}

//CALL WHEN CLOSING AN EMULATOR INSTANCE
void SharpboyCore::cleanup() {
	m_cartridge.reset();

	m_core.emu_ready = true;
	std::cout << "CLEANUP SUCCESSFUL. READY FOR NEW INSTANCE\n";
}

//execution
//EXECUTE ALL SINGLE STEP TESTS FOR NORMAL AND PREFIXED OPCODES
//HALT, STOP AND ILLGEAL OPCODES ARE NOT INCLUDED

//TODO ABSTRACT THIS WHOLE FUNCTION TO A SEPERATE CLASS FOR SST FUNCTIONALLITY
//WILL CALL ON SEPERATE THREAD AND ALLOW COMMANDS FROM CLI TO STILL BE RUN
//TAKE OVER MAIN THREAD WITH RESULTS AND SHOW THEM TO THE USER
void SharpboyCore::run_ssts(std::string sst_dir, bool background_thread) {
	//get a ptr for sst cart to load test data
	CartridgeSST* sst_cart = dynamic_cast<CartridgeSST*>(m_cartridge.get());
	if (!sst_cart) {
		std::cout << "UNABLE TO CAST TO SST CART RETURNING...\n";
		return;
	}
	else {
		std::cout << "SST CAST SUCCESSFUL\nSTARTING SSTs...\n";
	}

	//check if sst dir exists
	if (!std::filesystem::exists(sst_dir)) {
		std::cout << "SST BASE DIR DOESN'T EXIST!\n";
		return;
	}

	s_test_result t_result = {};
	std::vector<s_test_result> t_test_result = std::vector<s_test_result>();
	bool failed = false;

	//UNPREFIXED OPCODES
	//TODO ABSTRACT ALL THIS FILEREADING TO FILEREADER CLASS 
	for (int i = 0; i < SST_TEST_COUNT_NORMAL; i++) {
		std::filesystem::path test_path = sst_dir + "//" + sst_test_names_normal[i];
		if (!std::filesystem::exists(test_path)) {
			t_test_result.emplace_back(true, "FAILED TO FIND SST FILE", i);
			continue;
		}

		std::ifstream TEST(test_path);
		if (!TEST.is_open()) {
			t_test_result.emplace_back(true, "FAILED TO OPEN SST FILE", i);
		}

		nlohmann::json json;
		TEST >> json;

		std::unique_ptr<s_test_case> test = std::make_unique<s_test_case>();

		//COMPLETE EACH OF 1000 TEST FOR EACH OPCODE
		for (const auto test_case : json) {
			//RESET 
			test->clear_all();
			sst_cart->sst_reset();
			if (failed) {
				break;
			}

			//LOAD JSON
			auto test_name = test_case["name"].get<std::string>();
			auto initial_test_data = test_case["initial"];
			auto final_test_data = test_case["final"];
			auto cycles = test_case["cycles"];

			//SET NAME
			test->test_name = test_name;

			//SET REGISTERS
			for (int r = 0; r < 0x08; r++) {
				test->initial_registers[r] = initial_test_data[sst_register_names[r]];
				test->final_registers[r] = final_test_data[sst_register_names[r]];
			}

			//SET MEMORY
			for (auto mem_entry : initial_test_data["ram"]) {
				test->initial_memory.emplace_back(mem_entry[0], mem_entry[1]);
				sst_cart->unblocked_write(mem_entry[0], mem_entry[1]);
			}

			for (auto mem_entry : final_test_data["ram"]) {
				test->final_memory.emplace_back(mem_entry[0], mem_entry[1]);
			}

			//SET CYCLES
			for (auto cycle : cycles) {
				test->final_cycles.emplace_back(cycle[0], cycle[1], cycle[2].get<std::string>());
			}

			//EXECUTE TEST


			//COMPARE REGISTERS
			

			//COMPARE MEMORY
			for (s_test_mem_entry mem_entry : test->final_memory) {
				u8 expected_value = mem_entry.value;
				u8 got_value = sst_cart->read(mem_entry.address);

				if (expected_value == got_value) {
					t_test_result.emplace_back(true, std::format("TEST FAIL {} | MEMORY MISMATCH | EXP: {} GOT: {}", i, expected_value, got_value).c_str(), i);
					failed = true;
				}
			}

			//COMPARE CYCLES

			//SLEEP IF BG THREAD
			if (background_thread) {
				std::this_thread::sleep_for(std::chrono::microseconds(1));
			}
		}

		//ADD TEST SUCCESS OR TEST FAILED
		if (!failed) {
			t_test_result.emplace_back(false, std::format("PASSED TEST {}", i).c_str(), i);
			TEST.close();
		}
		else {
			failed = false;
		}
	}

	//OUTPUT RESULTS
	for (s_test_result result : t_test_result) {
		std::cout << result.message << "\n";
	}

	//RESET CART PTR
	sst_cart = nullptr;
}

//DEBUG GETTERS AND SETTERS
s_core_context* SharpboyCore::get_core_context() {
	return &m_core;
}