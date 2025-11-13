#pragma once
#include <format>

#include "FileReader.h"
#include "SST_Defs.h"
#include "NlohmannJSON/json.hpp"
#include "Common.h"
#include "Logger.h"

class Cartridge;
class CartridgeSST;
class CPU;

class SST_Tester {
public:
	//CONS DEST
	~SST_Tester();

	//INITIALISATION
	void init(std::string sst_path, Cartridge* cartridge_ptr, CPU* cpu_ptr);
	bool is_initialised() const;

	//EXECUTION
	bool check_test_exists(bool prefix, int test_opcode);
	bool run_test(bool prefix, int test_opcode);

	//RESULTS
	void clear_test_results();
	std::vector<s_test_result>* get_results();
	u16 get_last_cycle_address() const;
	u8 get_last_cycle_value() const;

	//CYCLES
	void add_cycle(u16 address, u8 value, std::string op);
	void reset_cycles();

private:
	//MEMBER VARIABLES
	bool m_test_initialised = false;
	std::string m_sst_dir_path = "";

	//PTR TO COMPONENTS
	CartridgeSST* m_cart = nullptr;
	CPU* m_cpu = nullptr;

	//TEST DATA
	std::unique_ptr<s_test_case> m_test_case = nullptr;
	std::vector<s_test_result> m_test_results = std::vector<s_test_result>();
	std::vector<s_test_cycle> m_subtest_cycles = std::vector<s_test_cycle>();

private:
	//MEMBER FUNCTIONS
	void init_reset();
};