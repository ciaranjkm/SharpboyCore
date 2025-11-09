#include "Testing.h"
#include "../Cartridges/SST_Cartridge.h"

SST_Tester::~SST_Tester() {
	test.reset();
	test = nullptr;

	test_results.clear();

	cartridge = nullptr;

	initialised = false;
}

void SST_Tester::init(std::string sst_path, CartridgeSST* cartridge_ptr) {
	if (!std::filesystem::exists(sst_path)) {
		std::cout << "SST BASE DIR DOESN'T EXIST!\n";
		initialised = false;
		return;
	}
	
	this->sst_path = sst_path;
	cartridge = cartridge_ptr;

	test = std::make_unique<s_test_case>();

	if (!cartridge || !test) {
		initialised = false;
	}
	else {
		initialised = true;
	}
}

bool SST_Tester::is_initialised() const {
	return initialised;
}

void SST_Tester::reset() {
	if (!initialised) {
		return;
	}

	cartridge->sst_reset();
	test->clear_all();
}

//execution
bool SST_Tester::check_test_exists(bool prefix, int test_opcode) {
	if (!prefix) {
		std::filesystem::path test_path = sst_path + "//" + sst_test_names_normal[test_opcode];
		if (!std::filesystem::exists(test_path)) {
			return false;
		}
	}
	else {
		std::filesystem::path test_path = sst_path + "//" + sst_test_names_prefixed[test_opcode];
		if (!std::filesystem::exists(test_path)) {
			return false;
		}
	}

	return true;
}

bool SST_Tester::run_test(bool prefix, int test_opcode) {
	if (!check_test_exists(prefix, test_opcode)) {
		test_results.emplace_back(true, std::format("FAIL | NO FILE {} {}", (prefix ? "cb " : ""), test_opcode), test_opcode);
		return false;
	}

	std::string test_file_name = sst_path +  "/" + (prefix ? sst_test_names_prefixed[test_opcode] : sst_test_names_normal[test_opcode]);
	std::fstream TEST(test_file_name);
	if (!TEST.is_open()) {
		test_results.emplace_back(true, std::format("FAIL | OPEN FILE {} {}", (prefix ? "cb " : ""), test_opcode), test_opcode);
		return false;
	}

	nlohmann::json json;
	TEST >> json;

	//COMPLETE EACH OF 1000 TEST FOR EACH OPCODE
	for (const auto test_case : json) {
		//RESET 
		test->clear_all();
		cartridge->sst_reset();

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
			cartridge->unblocked_write(mem_entry[0], mem_entry[1]);
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
			u8 got_value = cartridge->read(mem_entry.address);

			if (expected_value != got_value) {
				test_results.emplace_back(true, std::format("TEST FAIL {} | MEMORY MISMATCH | EXP: {} GOT: {}", test_opcode, expected_value, got_value).c_str(), test_opcode);
				return false;
			}
		}

		//COMPARE CYCLES

	}
		
	test_results.emplace_back(false, std::format("PASSED TEST {}", test_opcode).c_str(), test_opcode);
	TEST.close();
	return true;
}

void SST_Tester::clear_test_results() {
	test_results.clear();
}

std::vector<s_test_result>* SST_Tester::get_results() {
	return &test_results;
}