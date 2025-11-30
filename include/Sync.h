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
	uint64_t overrun_ticks = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> previous_time;
	std::chrono::time_point<std::chrono::high_resolution_clock> second_start;

	void start_clock() {
		overrun_ticks = 0;
		previous_time = std::chrono::high_resolution_clock::now();
		second_start = std::chrono::high_resolution_clock::now();
	}
};

struct SyncState {
	std::chrono::high_resolution_clock::time_point previous_time;
	std::chrono::high_resolution_clock::time_point start_time;
	int64_t overrun_ticks;
	int64_t total_target_ticks;
	int64_t total_actual_ticks;

	// Statistics for monitoring
	uint64_t frame_count;
	double avg_drift_cycles;
};


const uint64_t CPU_CLOCK = 4194304;

class Syncroniser {
public:
	~Syncroniser();

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
	SyncState m_sync = {};

};