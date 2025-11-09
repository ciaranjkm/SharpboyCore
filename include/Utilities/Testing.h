#pragma once
#include <format>

#include "FileReader.h"
#include "SST_Defs.h"
#include "NlohmannJSON/json.hpp"
#include "Common.h"

class CartridgeSST;

class SST_Tester {
public:
	~SST_Tester();

	void init(std::string sst_path, CartridgeSST* cartridge_ptr);

	bool is_initialised() const;
	void reset();

	//execution
	bool check_test_exists(bool prefix, int test_opcode);
	bool run_test(bool prefix, int test_opcode);

	void clear_test_results();
	std::vector<s_test_result>* get_results();

private:
	//control bools
	bool initialised = false;
	std::string sst_path = "";

	//ptr to components
	CartridgeSST* cartridge = nullptr;

	//test vars
	std::unique_ptr<s_test_case> test = nullptr;
	std::vector<s_test_result> test_results = std::vector<s_test_result>();
};