#include <Timing.h>

Timing::~Timing() {
	m_cpu = nullptr;
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
