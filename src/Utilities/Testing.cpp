#include "../include/Utilities/Testing.h"
#include "../include/Cartridges/SST_Cartridge.h"
#include "../include/CPU.h"

//CONS DEST
SST_Tester::~SST_Tester() {
	m_test_case.reset();
	m_test_case = nullptr;

	m_test_results.clear();

	m_cart = nullptr;
	m_cpu = nullptr;

	m_test_initialised = false;
}

//INITIALISATION
void SST_Tester::init(std::string sst_path, Cartridge* cartridge_ptr, CPU* cpu_ptr) {
	bool success = true;
	std::string error_message = "SST init failed. ";

	init_reset();

	//cast cartridge to sst_cart 
	m_cart = dynamic_cast<CartridgeSST*>(cartridge_ptr);
	if (!m_cart && success) {
		error_message.append("Cartridge cast failed");
		success = false;
	}

	//check for test path valid
	m_sst_dir_path = sst_path;
	if (!std::filesystem::exists(m_sst_dir_path) && success) {
		error_message.append("File path does not exist");
		success = false;
	}

	//setup cpu for sst mode and set ptr to this to add cycles to test results
	m_cpu = cpu_ptr;
	if (!m_cpu && success) {
		error_message.append("CPU given nullptr");
		success = false;
	}
	m_cpu->start_sst_mode(this);

	//setup test case 
	m_test_case = std::make_unique<s_test_case>();
	if (!m_test_case && success) {
		error_message.append("Test case struct creation failed");
		success = false;
	}

	if (!success) {
		msg_sst_error(error_message);
		init_reset();
		return;
	}

	m_test_initialised = true;
	msg_sst_status("SST object initialistion successful");
}

bool SST_Tester::is_initialised() const {
	return m_test_initialised;
}

/*
	TODO:: cleanup tester class and rename file and class. remove ex functions and add add_cycle func that can add cycle to the test from the cpu, stop cpu from owning cycles.
*/

//EXECUTION
bool SST_Tester::check_test_exists(bool prefix, int test_opcode) {
	if (!prefix) {
		std::filesystem::path test_path = m_sst_dir_path + "//" + sst_test_names_normal[test_opcode];
		if (!std::filesystem::exists(test_path)) {
			return false;
		}
	}
	else {
		std::filesystem::path test_path = m_sst_dir_path + "//" + sst_test_names_prefixed[test_opcode];
		if (!std::filesystem::exists(test_path)) {
			return false;
		}
	}

	return true;
}

bool SST_Tester::run_test(bool prefix, int test_opcode) {
	// CHECK IF TEST FILE EXISTS
	if (!check_test_exists(prefix, test_opcode)) {
		m_test_results.emplace_back(true, std::format("FAIL | NO FILE {} {}", (prefix ? "cb " : ""), test_opcode), test_opcode);
		return false;
	}

	// CHECK IF OPEN TEST FILE
	std::string test_file_name = m_sst_dir_path + "/" + (prefix ? sst_test_names_prefixed[test_opcode] : sst_test_names_normal[test_opcode]);
	std::fstream TEST(test_file_name);
	if (!TEST.is_open()) {
		m_test_results.emplace_back(true, std::format("FAIL | OPEN FILE {} {}", (prefix ? "cb " : ""), test_opcode), test_opcode);
		return false;
	}

	// PARSE JSON ONCE
	nlohmann::json json = nlohmann::json::parse(TEST);
	TEST.close();

	const char* reg_names[] = { "A", "F", "B", "C", "D", "E", "H", "L" };

	// COMPLETE EACH OF 1000 TESTS FOR EACH OPCODE
	for (const auto& test_case : json) {
		// RESET TEST CYCLES, CARTRIDGE MEMORY AND TEST CASE DATA
		reset_cycles();
		m_test_case->clear_all();
		m_cart->sst_reset();

		// LOAD JSON 
		const auto& test_name = test_case["name"].get<std::string>();
		const auto& initial_test_data = test_case["initial"];
		const auto& final_test_data = test_case["final"];
		const auto& cycles = test_case["cycles"];

		// SET NAME
		m_test_case->test_name = test_name;

		// SET REGISTERS
		for (int r = 0; r < 0x08; r++) {
			m_test_case->initial_registers[r] = initial_test_data[sst_register_names[r]];
			m_test_case->final_registers[r] = final_test_data[sst_register_names[r]];
		}
		m_test_case->initial_pc = initial_test_data["pc"];
		m_test_case->initial_sp = initial_test_data["sp"];
		m_cpu->reset_regs(m_test_case->initial_registers, m_test_case->initial_sp, m_test_case->initial_pc);

		// SET MEMORY
		const auto& initial_ram = initial_test_data["ram"];
		const auto& final_ram = final_test_data["ram"];
		m_test_case->initial_memory.reserve(initial_ram.size());
		m_test_case->final_memory.reserve(final_ram.size());

		for (const auto& mem_entry : initial_ram) {
			m_test_case->initial_memory.emplace_back(mem_entry[0], mem_entry[1]);
			m_cart->unblocked_write(mem_entry[0], mem_entry[1]);
		}
		for (const auto& mem_entry : final_ram) {
			m_test_case->final_memory.emplace_back(mem_entry[0], mem_entry[1]);
		}

		// SET CYCLES
		m_test_case->final_cycles.reserve(cycles.size());
		for (const auto& cycle : cycles) {
			m_test_case->final_cycles.emplace_back(cycle[0], cycle[1], cycle[2].get<std::string>());
		}

		// EXECUTE TEST
		m_cpu->execute_next_instruction();

		// COMPARE REGISTERS
		s_registers* final_regs = m_cpu->get_registers();
		u8 actual_regs[] = { final_regs->a, final_regs->f, final_regs->b, final_regs->c, final_regs->d, final_regs->e, final_regs->h, final_regs->l };

		for (int i = 0; i < 8; i++) {
			if (actual_regs[i] != m_test_case->final_registers[i]) {
				m_test_results.emplace_back(true, std::format("REG FAIL @{} | EXP {} GOT {}", reg_names[i], m_test_case->final_registers[i], actual_regs[i]), test_opcode);
				return false;
			}
		}

		// COMPARE MEMORY
		for (const auto& mem_entry : m_test_case->final_memory) {
			u8 expected_value = mem_entry.value;
			u8 got_value = m_cart->read(mem_entry.address);

			if (expected_value != got_value) {
				m_test_results.emplace_back(true, std::format("MEMORY MISMATCH | EXP: {} GOT: {}", expected_value, got_value), test_opcode);
				return false;
			}
		}

		// COMPARE CYCLES
		if (m_subtest_cycles.size() != m_test_case->final_cycles.size()) {
			m_test_results.emplace_back(true, std::format("CYCLES SIZE FAIL | EXP: {} GOT: {}", m_test_case->final_cycles.size(), m_subtest_cycles.size()), test_opcode);
			return false;
		}

		for (size_t c = 0; c < m_test_case->final_cycles.size(); c++) {
			const auto& expected_cycle = m_test_case->final_cycles[c];
			const auto& actual_cycle = m_subtest_cycles[c];

			if (actual_cycle.address != expected_cycle.address) {
				m_test_results.emplace_back(true, std::format("CYCLES ADDR FAIL | EXP: {} GOT: {}", expected_cycle.address, actual_cycle.address), test_opcode);
				return false;
			}
			if (actual_cycle.value != expected_cycle.value) {
				m_test_results.emplace_back(true, std::format("CYCLES VAL FAIL | EXP: {} GOT: {}", expected_cycle.value, actual_cycle.value), test_opcode);
				return false;
			}
			if (actual_cycle.operation != expected_cycle.operation) {
				m_test_results.emplace_back(true, std::format("CYCLES OP FAIL | EXP: {} GOT: {}", expected_cycle.operation, actual_cycle.operation), test_opcode);
				return false;
			}
		}
	}

	m_test_results.emplace_back(false, std::format("PASSED TEST {}", test_opcode), test_opcode);
	return true;
}

void SST_Tester::clear_test_results() {
	m_test_results.clear();
}

std::vector<s_test_result>* SST_Tester::get_results() {
	return &m_test_results;
}

void SST_Tester::add_cycle(bool idle, u16 address, u8 value, std::string op) {
	if (idle && !m_subtest_cycles.empty()) {
		u16 last_address = m_subtest_cycles.back().address;
		u8 last_value = m_subtest_cycles.back().value;
		m_subtest_cycles.emplace_back(last_address, last_value, "---");

		return;
	}

	m_subtest_cycles.emplace_back(address, value, op);
}

void SST_Tester::reset_cycles() {
	m_subtest_cycles.clear();
}

//MEMBER FUNCTIONS
void SST_Tester::init_reset() {
	m_cart = nullptr;
	m_cpu = nullptr;

	if (m_test_case) {
		m_test_case.reset();
	}

	m_sst_dir_path.clear();
	m_test_initialised = false;

	m_test_results.clear();
	m_subtest_cycles.clear();
}