#include <Utilities/SST.h>

//INITIALISATION
SST::SST(std::filesystem::path sst_path, bool prefixed) {
	FileReader::update_path(path_sst, sst_path);

	this->prefixed = prefixed;
	this->sst_path = sst_path;

	tCPU = std::make_unique<testCPU>();
	if (!tCPU) {
		return;
	}

	initialised.store(true);
}

SST::~SST() {
	tCPU.reset();
	tCPU = nullptr;
}

//EXECUTION
void SST::run() {
	if (!initialised.load()) {
		//failed init cant run
		return;
	}

	completed_tests.store(false);

	//LOOP SMALL TEST COUNT 0-16
	for (int i = 0; i < SMALL_TEST_COUNT; i++) {
		std::string test_name = prefixed ? sst_test_names_prefixed[i] : sst_test_names_normal[i];

		//CREATE RESULT FOR TEST
		s_test_result& result = m_results[i];
		result.test_num = i;
		result.prefixed = prefixed;
		result.msg = std::format("{} | ", test_name);
		completed_tests_count++;

		//CHECK FOR TEST FILE AND READ IT
		std::string test_file_name = std::format("{}", prefixed ? sst_test_names_prefixed[i] : sst_test_names_normal[i]);
		if (test_file_name == invalid_json_file_name) {
			result.msg.append(std::format("INVALID OPCODE {:#x}", i));
			result.result = false;
			continue;
		}
		
		std::string file_name = std::format("{}/{}", sst_path.filename().string(), test_file_name);

		if (!std::filesystem::exists(file_name)) {
			result.result = false;
			result.msg.append(std::format("COULDN'T FIND TEST FILE {}", file_name));
			continue;
		}

		std::ifstream test(file_name);
		nlohmann::json test_json = nlohmann::json::parse(test);
		test.close();
		
		//LOOP 1000 TIMES
		for (const auto& test_case : test_json) {
			tCPU->reset_for_next_test();

			const auto& test_initial = test_case["initial"];
			const auto& test_final = test_case["final"];
			const auto& test_cycles = test_case["cycles"];

			//UPDATE REGISTERS
			s_registers initial_regs = {
				.a = test_initial["a"],
				.f = test_initial["f"],
				.b = test_initial["b"],
				.c = test_initial["c"],
				.d = test_initial["d"],
				.e = test_initial["e"],
				.h = test_initial["h"],
				.l = test_initial["l"],
				.pc = test_initial["pc"],
				.sp = test_initial["sp"],
			};
			s_registers final_regs = {
				.a = test_final["a"],
				.f = test_final["f"],
				.b = test_final["b"],
				.c = test_final["c"],
				.d = test_final["d"],
				.e = test_final["e"],
				.h = test_final["h"],
				.l = test_final["l"],
				.pc = test_final["pc"],
				.sp = test_final["sp"],
			};

			tCPU->update_registers_state(initial_regs);

			//UPDATE MEMORY
			std::vector<s_test_memory_pair> initial_mem = std::vector<s_test_memory_pair>();
			std::vector<s_test_memory_pair> final_mem = std::vector<s_test_memory_pair>();

			for (const auto& mem_entry : test_initial["ram"]) {
				initial_mem.emplace_back(mem_entry[0], mem_entry[1]);
			}
			for (const auto& mem_entry : test_final["ram"]) {
				final_mem.emplace_back(mem_entry[0], mem_entry[1]);
			}
			tCPU->update_memory_state(initial_mem);

			//GET CYCLES
			std::vector<s_test_cycle> final_cycles = std::vector<s_test_cycle>();
			final_cycles.reserve(test_cycles.size());
			for (const auto& cycle : test_cycles) {
				final_cycles.emplace_back(cycle[0], cycle[1], cycle[2].get<std::string>());
			}

			//EXECUTE TEST
			tCPU->step();
			tCPU->get_test_result(result, final_regs, final_mem, final_cycles);

			if (!result.result) {
				break;
			}
		}

		if (result.result) {
			result.msg.append("TEST PASSED");
		}

		m_results[i] = result;
	}

	completed_tests.store(true);
}

//RESULTS
int SST::get_completed_tests_count() const {
	return completed_tests_count.load();
}

std::array<s_test_result, SMALL_TEST_COUNT>* SST::get_results() {
	return &m_results;
}

bool SST::is_test_complete() const {
	return completed_tests.load();
}
