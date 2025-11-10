#include "../include/Utilities/Testing.h"
#include "../include/Cartridges/SST_Cartridge.h"
#include "../include/CPU.h"

//CONS DEST
SST_Tester::~SST_Tester() {
	m_test_case.reset();
	m_test_case = nullptr;

	m_test_results.clear();

	cartridge = nullptr;
	cpu = nullptr;

	m_test_initialised = false;
}

//INITIALISATION
void SST_Tester::init(std::string sst_path, CartridgeSST* cartridge_ptr, CPU* cpu_ptr) {
	if (!std::filesystem::exists(sst_path)) {
		std::cout << "SST BASE DIR DOESN'T EXIST!\n";
		m_test_initialised = false;
		return;
	}
	
	this->m_sst_dir_path = sst_path;
	cartridge = cartridge_ptr;
	cpu = cpu_ptr;

	m_test_case = std::make_unique<s_test_case>();

	if (!cartridge || !m_test_case) {
		m_test_initialised = false;
	}
	else {
		m_test_initialised = true;
	}
}

bool SST_Tester::is_initialised() const {
	return m_test_initialised;
}

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
	//CHECK IF TEST FILE EXISTS
	if (!check_test_exists(prefix, test_opcode)) {
		m_test_results.emplace_back(true, std::format("FAIL | NO FILE {} {}", (prefix ? "cb " : ""), test_opcode), test_opcode);
		return false;
	}

	//CHECK IF OPEN TEST FILE
	std::string test_file_name = m_sst_dir_path +  "/" + (prefix ? sst_test_names_prefixed[test_opcode] : sst_test_names_normal[test_opcode]);
	std::fstream TEST(test_file_name);
	if (!TEST.is_open()) {
		m_test_results.emplace_back(true, std::format("FAIL | OPEN FILE {} {}", (prefix ? "cb " : ""), test_opcode), test_opcode);
		return false;
	}

	//PUSH INTO JSON OBJECT
	nlohmann::json json;
	TEST >> json;

	//COMPLETE EACH OF 1000 TEST FOR EACH OPCODE
	for (const auto test_case : json) {
		//RESET TEST CYCLES, CARTRIDGE MEMORY AND TEST CASE DATA
		reset_cycles();
		m_test_case->clear_all();
		cartridge->sst_reset();

		//LOAD JSON
		auto test_name = test_case["name"].get<std::string>();
		auto initial_test_data = test_case["initial"];
		auto final_test_data = test_case["final"];
		auto cycles = test_case["cycles"];

		//SET NAME
		m_test_case->test_name = test_name;

		//SET REGISTERS
		for (int r = 0; r < 0x08; r++) {
			m_test_case->initial_registers[r] = initial_test_data[sst_register_names[r]];
			m_test_case->final_registers[r] = final_test_data[sst_register_names[r]];
		}
		m_test_case->initial_pc = initial_test_data["pc"];
		m_test_case->initial_sp = initial_test_data["sp"];
		cpu->reset_regs(m_test_case->initial_registers, m_test_case->initial_sp, m_test_case->initial_pc);

		//SET MEMORY
		for (auto mem_entry : initial_test_data["ram"]) {
			m_test_case->initial_memory.emplace_back(mem_entry[0], mem_entry[1]);
			cartridge->unblocked_write(mem_entry[0], mem_entry[1]);
		}
		for (auto mem_entry : final_test_data["ram"]) {
			m_test_case->final_memory.emplace_back(mem_entry[0], mem_entry[1]);
		}

		//SET CYCLES
		for (auto cycle : cycles) {
			m_test_case->final_cycles.emplace_back(cycle[0], cycle[1], cycle[2].get<std::string>());
		}

		//EXECUTE TEST
		cpu->execute_next_instruction();

		//COMPARE REGISTERS
		s_registers* final_regs = cpu->get_registers();
		std::string register_fail = "";
		u8 expected = 0x00;
		u8 got = 0x00;

		if (final_regs->a != m_test_case->final_registers[0]) {
			expected = m_test_case->final_registers[0];
			got = final_regs->a;

			register_fail = std::format("REG FAIL @A | EXP {} GOT {}", expected, got);
		}
		else if (final_regs->f != m_test_case->final_registers[1]) {
			expected = m_test_case->final_registers[1];
			got = final_regs->f;
			register_fail = std::format("REG FAIL @F | EXP {} GOT {}", expected, got);
		}
		else if (final_regs->b != m_test_case->final_registers[2]) {
			expected = m_test_case->final_registers[2];
			got = final_regs->b;
			register_fail = std::format("REG FAIL @B | EXP {} GOT {}", expected, got);
		}
		else if (final_regs->c != m_test_case->final_registers[3]) {
			expected = m_test_case->final_registers[3];
			got = final_regs->c;
			register_fail = std::format("REG FAIL @C | EXP {} GOT {}", expected, got);
		}
		else if (final_regs->d != m_test_case->final_registers[4]) {
			expected = m_test_case->final_registers[4];
			got = final_regs->d;
			register_fail = std::format("REG FAIL @D | EXP {} GOT {}", expected, got);
		}
		else if (final_regs->e != m_test_case->final_registers[5]) {
			expected = m_test_case->final_registers[5];
			got = final_regs->e;
			register_fail = std::format("REG FAIL @E | EXP {} GOT {}", expected, got);
		}
		else if (final_regs->h != m_test_case->final_registers[6]) {
			expected = m_test_case->final_registers[6];
			got = final_regs->h;
			register_fail = std::format("REG FAIL @H | EXP {} GOT {}", expected, got);
		}
		else if (final_regs->l != m_test_case->final_registers[7]) {
			expected = m_test_case->final_registers[7];
			got = final_regs->l;
			register_fail = std::format("REG FAIL @L | EXP {} GOT {}", expected, got);
		}

		if (register_fail != "") {
			m_test_results.emplace_back(true, register_fail, test_opcode);
			return false;
		}

		//COMPARE MEMORY
		for (s_test_mem_entry mem_entry : m_test_case->final_memory) {
			u8 expected_value = mem_entry.value;
			u8 got_value = cartridge->read(mem_entry.address);

			if (expected_value != got_value) {
				m_test_results.emplace_back(true, std::format("MEMORY MISMATCH | EXP: {} GOT: {}", test_opcode, expected_value, got_value).c_str(), test_opcode);
				return false;
			}
		}

		//COMPARE CYCLES
		if (m_subtest_cycles.size() != m_test_case->final_cycles.size()) {
			m_test_results.emplace_back(true, std::format("CYCLES SIZE FAIL | EXP: {} GOT: {}", m_subtest_cycles.size(), m_test_case->final_cycles.size()), test_opcode);
			return false;
		}
		for (int c = 0; c < m_test_case->final_cycles.size(); c++) {
			//check address
			if (m_subtest_cycles[c].address != m_test_case->final_cycles[c].address) {
				m_test_results.emplace_back(true, std::format("CYCLES ADDR FAIL | EXP: {} GOT: {}", m_test_case->final_cycles[c].address, m_subtest_cycles[c].address), test_opcode);
				return false;
			}
			if (m_subtest_cycles[c].value != m_test_case->final_cycles[c].value) {
				m_test_results.emplace_back(true, std::format("CYCLES VAL FAIL | EXP: {} GOT: {}", m_test_case->final_cycles[c].value, m_subtest_cycles[c].value), test_opcode);
				return false;
			}
			if (m_subtest_cycles[c].operation != m_test_case->final_cycles[c].operation) {
				m_test_results.emplace_back(true, std::format("CYCLES OP FAIL | EXP: {} GOT: {}", m_test_case->final_cycles[c].operation, m_subtest_cycles[c].operation), test_opcode);
				return false;
			}
		}
	}
		
	m_test_results.emplace_back(false, std::format("PASSED TEST {}", test_opcode).c_str(), test_opcode);
	TEST.close();
	return true;
}

void SST_Tester::clear_test_results() {
	m_test_results.clear();
}

std::vector<s_test_result>* SST_Tester::get_results() {
	return &m_test_results;
}

u16 SST_Tester::get_last_cycle_address() const {
	if(m_subtest_cycles.size() == 0) {
		return 0x0000;
	}

	return m_subtest_cycles.back().address;
}

u8 SST_Tester::get_last_cycle_value() const {
	if (m_subtest_cycles.size() == 0) {
		return 0x00;
	}
	return m_subtest_cycles.back().value;
}

void SST_Tester::add_cycle(u16 address, u8 value, std::string op) {
	m_subtest_cycles.emplace_back(address, value, op);
}

void SST_Tester::reset_cycles() {
	m_subtest_cycles.clear();
}