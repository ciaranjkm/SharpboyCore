#include <Timing.h>

Timing::~Timing() {
	m_cpu = nullptr;
	m_ppu = nullptr;
}

void Timing::attach_components(CPU* cpu, PPU* ppu) {
	m_cpu = cpu;
	m_ppu = ppu;
}

void Timing::real_ticks(int ticks) {
	for (int i = 0; i < ticks; i++) {
		//advance other components
		m_ppu->tick();
	}
}

void Timing::real_cycle() {
	for (int i = 0; i < 4; i++) {
		//advance other components
		m_ppu->tick();
	}
}
