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
private:
	CPU* m_cpu = nullptr;
};