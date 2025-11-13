#include "../include/Timing.h"
#include "../include/CPU.h"

Timing::~Timing() {
	m_cpu = nullptr;
	sst_tester = nullptr;
}

void Timing::attach_components(CPU* cpu) {
	m_cpu = cpu;
}

void Timing::real_ticks(int ticks) {
	for (int i = 0; i < ticks; i++) {
		//advance other components
	}
}

void Timing::real_cycle() {
	for (int i = 0; i < 4; i++) {
		//advance other components
	}
}

//sst
void Timing::set_tester_ptr(SST_Tester* tester) {
	sst_tester = tester;
}

void Timing::set_sst_mode() {
	sst_mode = true;
}

void Timing::reset_sst_mode() {
	sst_mode = false;
}