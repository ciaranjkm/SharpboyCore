#pragma once
#include <array>
#include <vector>
#include <format>

#include "bCPU.h"

struct s_test_result {
	bool result = true;
	bool prefixed = false;
	int test_num = 0;
	std::string msg = "";
};
struct s_test_cycle {
	u16 address = 0x0000;
	u8 value = 0x00;
	std::string op = "---";
};
struct s_test_memory_pair {
	u16 address = 0x0000;
	u8 value = 0x00;
};

const int SST_MEMORY_SIZE = 0x10000; //64kb flag ram
const std::array<std::string, 8> register_names = { "A", "F", "B", "C", "D", "E", "H", "L" };

class testCPU : public bCPU {
public:
	//TEST INITIALISATION
	void reset_for_next_test();
	void update_registers_state(s_registers regs);
	void update_memory_state(const std::vector<s_test_memory_pair>& memory);

	//TEST RESULTS
	void get_test_result(s_test_result& test_result, s_registers expected_regs, const std::vector<s_test_memory_pair>& expected_memory, const std::vector<s_test_cycle>& expected_cycles);

private:
	//MEMBER VARIABLES
	std::array<u8, SST_MEMORY_SIZE> m_memory = std::array<u8, SST_MEMORY_SIZE>();
	std::vector<s_test_cycle> m_test_cycles = std::vector<s_test_cycle>();

private:
	//MEMBER FUNCTIONS
	void check_registers(s_test_result& test_result, s_registers expected_regs);
	void check_memory(s_test_result& test_result, const std::vector<s_test_memory_pair>& expected_memory);
	void check_cycles(s_test_result& test_result, const std::vector<s_test_cycle>& expected_cycles);

	std::array<u8, 8> convert_registers(s_registers regs);

	//VIRTUAL FUNCTIONS
	u8 read(u16 address) override;
	void write(u16 address, u8 value) override;

	u8 read_pc(bool read_interrupt = false) override;
	u16 read_pc_short() override;

	void tick_components(int cycles) override;
	void idle_cycle() override;
};