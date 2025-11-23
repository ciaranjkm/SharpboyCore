#pragma once

#include <chrono>
#include <thread>

class ComponentManager;
class CPU;
class PPU;
class Timer;

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

struct s_basic_sync {
	int overrun_ticks = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> previous_time;
	std::chrono::time_point<std::chrono::high_resolution_clock> second_start;

	void start_clock() {
		overrun_ticks = 0;
		previous_time = std::chrono::high_resolution_clock::now();
		second_start = std::chrono::high_resolution_clock::now();
	}
};

const uint64_t CPU_CLOCK = 4194304;

class Syncroniser {
public:
	~Syncroniser();

	void start_syncroniser();
	void reset_syncroniser();

	int advance_cycles();

	void attach_components(ComponentManager* comp_manager);

	void real_ticks(int ticks);
	void real_cycle();
private:
	CPU* m_cpu = nullptr;
	PPU* m_ppu = nullptr;
	Timer* m_timer = nullptr;

	s_simple_timer s_clock_timer = {};

	int total_ticks = 0;

	//SYNC OPTIONS FOR TIMING (GENERAL TIMING (CYCLES COUNT / VSYNC) OR AUDIO)
	s_basic_sync m_sync = {};

};