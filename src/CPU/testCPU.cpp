#include <CPU/testCPU.h>

//TEST INITIALISTION
void testCPU::reset_for_next_test() {
	m_registers = {};
	m_memory.fill(0x00);
	m_test_cycles.clear();
}

void testCPU::update_registers_state(s_registers reg) {
	m_registers = reg;
}

void testCPU::update_memory_state(const std::vector<s_test_memory_pair>& memory) {
	for (s_test_memory_pair mem_pair : memory) {
		m_memory[mem_pair.address] = mem_pair.value;
	}
}

//TEST RESULTS
void testCPU::get_test_result(s_test_result& test_result, s_registers expected_regs, const std::vector<s_test_memory_pair>& expected_memory, const std::vector<s_test_cycle>& expected_cycles) {
	check_registers(test_result, expected_regs);
	check_memory(test_result, expected_memory);
	check_cycles(test_result, expected_cycles);
}

//MEMBER FUNCTIONS
void testCPU::check_registers(s_test_result& test_result, s_registers expected_regs) {
	if (!test_result.result) {
		//already failed
		return;
	}

	std::string fail_prefix = "REGISTER MISMATCH @";

	std::array<u8, 8> final = convert_registers(m_registers);
	std::array<u8, 8> expected = convert_registers(expected_regs);

	std::string fail_msg = "";
	for (int i = 0; i < 8; i++) {
		if (final[i] != expected[i]) {
			fail_msg = std::format("{} | GOT: {:#02x} EXP: {:#02x}", register_names[i], final[i], expected[i]);

			test_result.result = false;
			test_result.msg.append(std::format(" {}{}", fail_prefix, fail_msg));
			return;
		}
	}

	if (m_registers.pc != expected_regs.pc) {
		fail_msg = std::format("PC | GOT: {:#04x} EXP: {:#04x}", m_registers.pc, expected_regs.pc);

		test_result.result = false;
		test_result.msg.append(std::format(" {}{}", fail_prefix, fail_msg));

		return;
	}
	if (m_registers.sp != expected_regs.sp) {
		fail_msg = std::format("SP | GOT: {:#04x} EXP: {:#04x}", m_registers.sp, expected_regs.sp);

		test_result.result = false;
		test_result.msg.append(std::format(" {}{}", fail_prefix, fail_msg));

		return;
	}
}

void testCPU::check_memory(s_test_result& test_result, const std::vector<s_test_memory_pair>& expected_memory) {
	if (!test_result.result) {
		//already failed
		return;
	}

	std::string fail_prefix = "MEMORY MISMATCH @";

	for (s_test_memory_pair mem_pair : expected_memory) {
		u8 got_value = m_memory[mem_pair.address];

		if (got_value != mem_pair.value) {
			std::string fail_msg = std::format("{:02x} | GOT: {:#02x} EXP: {:#02x}", mem_pair.address, got_value, mem_pair.value);
			test_result.msg.append(std::format(" {}{}", fail_prefix, fail_msg));
			test_result.result = false;

			return;
		}
	}
}

void testCPU::check_cycles(s_test_result& test_result, const std::vector<s_test_cycle>& expected_cycles) {
	if (!test_result.result) {
		//already failed
		return;
	}
	
	std::string fail_prefix = "CYCLES MISMATCH";
	
	if (m_test_cycles.size() != expected_cycles.size()) {
		std::string fail_msg = std::format(" {} IN SIZE | GOT: {} EXP: {}", fail_prefix, m_test_cycles.size(), expected_cycles.size());

		test_result.result = false;
		test_result.msg.append(fail_msg);

		return;
	}

	for (size_t c = 0; c < expected_cycles.size(); c++) {
		s_test_cycle expected_cycle = expected_cycles[c];
		s_test_cycle actual_cycle = m_test_cycles[c];

		if (actual_cycle.address != expected_cycle.address) {
			std::string fail_msg = std::format(" {} IN ADDRESS | GOT: {:#04x} EXP: {:#04x}", fail_prefix, actual_cycle.address, expected_cycle.address);

			test_result.result = false;
			test_result.msg.append(fail_msg);

			return;
		}
		if (actual_cycle.value != expected_cycle.value) {
			std::string fail_msg = std::format(" {} IN VALUE | GOT: {:#02x} EXP: {:#02x}", fail_prefix, actual_cycle.value, expected_cycle.value);

			test_result.result = false;
			test_result.msg.append(fail_msg);

			return;
		}
		if (actual_cycle.op != expected_cycle.op) {
			std::string fail_msg = std::format(" {} IN OPERATION | GOT: {} EXP: {}", fail_prefix, actual_cycle.op, expected_cycle.op);

			test_result.result = false;
			test_result.msg.append(fail_msg);

			return;
		}
	}
}

std::array<u8, 8> testCPU::convert_registers(s_registers regs) {
	std::array<u8, 8> new_regs = { regs.a, regs.f, regs.b, regs.c, regs.d, regs.e, regs.h, regs.l };
	return new_regs;
}

void testCPU::add_cycle(u16 address, u8 value, std::string op) {
	m_test_cycles.emplace_back(address, value, op);
}

//VIRTUAL FUNCTIONS
u8 testCPU::read(u16 address) {
	u8 value = m_memory[address];
	add_cycle(address, value, "r-m");
	return value;
}

void testCPU::write(u16 address, u8 value) {
	m_memory[address] = value;
	add_cycle(address, value, "-wm");
}

u8 testCPU::read_pc(bool read_interrupt) {
	u8 value = read(m_registers.pc++);
	return value;
}

u16 testCPU::read_pc_short() {
	u8 low = read(m_registers.pc++); 
	u8 high = read(m_registers.pc++);

	return (high << 8) | low;
}

void testCPU::tick_components(int cycles) {
	//not needed for testing
}

void testCPU::idle_cycle() {
	if (!(m_test_cycles.size() > 0)) {
		return;
	}

	u16 address = m_test_cycles.back().address;
	u8 value = m_test_cycles.back().value;
	add_cycle(address, value, "---");
}