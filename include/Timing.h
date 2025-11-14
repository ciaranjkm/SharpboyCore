#pragma once

#include <chrono>
#include <thread>

#include "Utilities/Testing.h"

class CPU;

class Timing {
public:
	~Timing();

	void attach_components(CPU* cpu);

	void real_ticks(int ticks);
	void real_cycle();

	//sst 
	void set_tester_ptr(SST* tester);
	void set_sst_mode();
	void reset_sst_mode();

private:
	CPU* m_cpu = nullptr;
	
	bool sst_mode = false;
	SST* sst_tester = nullptr;
};