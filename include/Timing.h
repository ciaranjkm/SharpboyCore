#pragma once

#include <chrono>
#include <thread>

#include "PPU.h"

class CPU;

class Timing {
public:
	~Timing();

	void attach_components(CPU* cpu, PPU* ppu);

	void real_ticks(int ticks);
	void real_cycle();
private:
	CPU* m_cpu = nullptr;
	PPU* m_ppu = nullptr;
};