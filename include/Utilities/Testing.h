#pragma once
#include <format>
#include <filesystem>
#include <atomic>

#include "NlohmannJSON/json.hpp"
#include "SST_Defs.h"
#include "FileReader.h"

#include "../testCPU.h"

const int SMALL_TEST_COUNT = 256;

class SST {
public:
	SST(std::string sst_path, int start_test, bool prefixed);
	~SST();
	
	//EXECUTION
	void run();

	//RESULTS
	std::array<s_test_result, SMALL_TEST_COUNT>* get_results();
	bool is_test_complete() const;

private:
	std::atomic_bool initialised = false;
	std::atomic_bool completed_tests = false;

	std::unique_ptr<testCPU> tCPU = nullptr;

	std::string sst_path = "";
	int start_test = 0;
	bool prefixed = false;

	std::array<s_test_result, SMALL_TEST_COUNT> m_results = std::array<s_test_result, SMALL_TEST_COUNT>();

private:

};