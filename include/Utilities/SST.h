#pragma once
#include <format>
#include <filesystem>
#include <atomic>

#include "../CPU/testCPU.h"

#include "NlohmannJSON/json.hpp"
#include "SST_Defs.h"
#include "FileReader.h"

const int SMALL_TEST_COUNT = 256;

class SST {
public:
	//INITIALISTAION
	SST(std::string sst_path, int start_test, bool prefixed);
	~SST();
	
	//EXECUTION
	void run();

	//RESULTS
	int get_completed_tests_count() const;

	std::array<s_test_result, SMALL_TEST_COUNT>* get_results();
	bool is_test_complete() const;

private:
	//MEMBER VARIABLES
	std::atomic_bool initialised = false;
	std::atomic_bool completed_tests = false;
	std::atomic<int> completed_tests_count = 0;

	std::string sst_path = "";
	int start_test = 0;
	
	bool prefixed = false;

	std::array<s_test_result, SMALL_TEST_COUNT> m_results = std::array<s_test_result, SMALL_TEST_COUNT>();

	//PTR TO CPU OBJECT
	std::unique_ptr<testCPU> tCPU = nullptr;
};