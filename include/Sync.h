#pragma once

#include <chrono>
#include <thread>

#include "PPU.h"

class CPU;

struct s_simple_timer {
	bool timer_active = false;
	std::chrono::time_point<std::chrono::steady_clock> current_time;
	std::chrono::time_point<std::chrono::steady_clock> previous_time;

	double elapsed_time = 0;

	s_simple_timer() {
		previous_time = std::chrono::high_resolution_clock::now();
		current_time = std::chrono::high_resolution_clock::now();

		calculate_elapsed();
	}

	void get_current_time() {
		previous_time = current_time;
		current_time = std::chrono::high_resolution_clock::now();
	}

	void calculate_elapsed() {
		auto duration = current_time - previous_time;
		elapsed_time = std::chrono::duration<double>(duration).count();
	}

	double get_elapsed() const {
		return elapsed_time;
	}
};

const double CPU_CLOCK = 4194304.0;

class Syncroniser {
public:
	~Syncroniser();

	int advance_cycles();

	void attach_components(CPU* cpu, PPU* ppu);

	void real_ticks(int ticks);
	void real_cycle();
private:
	CPU* m_cpu = nullptr;
	PPU* m_ppu = nullptr;

	s_simple_timer s_clock_timer = {};
	int m_overrun_cycles = 0;
};