#pragma once

#include "Common.h"
#include <array>
#include <vector>
#include <string>
#include <format>

const int SST_TEST_COUNT_NORMAL = 1;
const int SST_TEST_COUNT_CB = 1;

const int SST_MEMORY_SIZE = 0x10000;

static const std::array<std::string, SST_TEST_COUNT_NORMAL> sst_test_names_normal = {
	"00.json"
};

static const std::array<std::string, SST_TEST_COUNT_CB> sst_test_names_prefixed = {
	"cb 00.json"
};

static const std::array<std::string, 0x08> sst_register_names = {
	"a", "f", "b", "c", "d", "e", "h", "l"
};

struct s_test_result {
	bool failed = false;
	std::string message = "";

	int test_index = 0;
};

struct s_test_cycle {
	u16 address;
	u8 value;
	std::string operation;
};

struct s_test_mem_entry {
	u16 address;
	u8 value;
};

inline static s_test_cycle create_test_cycle(u16 address, u8 value, std::string operation) {
	return s_test_cycle(address, value, operation);
}

struct s_test_case {
	void clear_all() {
		initial_memory.clear();
		final_memory.clear();

		initial_registers.fill(0x00);
		final_registers.fill(0x00);

		initial_pc = 0x0000;
		final_pc = 0x0000;

		initial_sp = 0x0000;
		final_sp = 0x0000;

		final_cycles.clear();
	}

	std::string test_name;

	std::vector<s_test_mem_entry> initial_memory = std::vector<s_test_mem_entry>();
	std::vector<s_test_mem_entry> final_memory = std::vector<s_test_mem_entry>();

	std::array<u8, 0x08> initial_registers = std::array<u8, 0x08>();
	std::array<u8, 0x08> final_registers = std::array<u8, 0x08>();

	u16 initial_pc;
	u16 final_pc;

	u16 initial_sp;
	u16 final_sp;

	std::vector<s_test_cycle> final_cycles = std::vector<s_test_cycle>();
};