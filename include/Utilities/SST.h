#pragma once
#include <format>
#include <filesystem>
#include <atomic>

#include "../Components/CPU/testCPU.h"

#include "NlohmannJSON/json.hpp"
#include "SST_Defs.h"
#include "FileReader.h"

const int SMALL_TEST_COUNT = 256;

/*
	A class to run and execute single step tests. Everything needed is contained within this class using a test CPU to execute and report
	back on instructions. The test CPU will output the results to the console with the fail reason test and text number, this isnt usable
	in my GUI version, only in CLI as I was only using this to develop the CPU. 

	You can make a global version of this class and use it with the get_results() function to use in a GUI version.
*/

class SST {
public:
	//INITIALISTAION
	SST(std::filesystem::path sst_path, bool prefixed);
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

	std::filesystem::path sst_path = "";
	
	bool prefixed = false;

	std::array<s_test_result, SMALL_TEST_COUNT> m_results = std::array<s_test_result, SMALL_TEST_COUNT>();

	//PTR TO CPU OBJECT
	std::unique_ptr<testCPU> tCPU = nullptr;
};