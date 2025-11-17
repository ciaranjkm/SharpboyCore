#include <Sync.h>
#include <CPU/CPU.h>

Syncroniser::~Syncroniser() {
	m_cpu = nullptr;
	m_ppu = nullptr;
}

int Syncroniser::advance_cycles() {
    /*const double target_frame_time = 1.0 / 59.73;
    const int cycles_per_frame = 70224;

    int cycles_completed = 0;

    while (cycles_completed < cycles_per_frame) {
        cycles_completed += m_cpu->execute_next_instruction();
    }

    s_clock_timer.get_current_time();
    s_clock_timer.calculate_elapsed();
    double actual_frame_time = s_clock_timer.get_elapsed();

    if (actual_frame_time < target_frame_time) {
        double sleep_duration = target_frame_time - actual_frame_time;
        std::this_thread::sleep_for(std::chrono::duration<double>(sleep_duration));
    }

    s_clock_timer.current_time = std::chrono::steady_clock::now();
    s_clock_timer.previous_time = s_clock_timer.current_time;

    return cycles_completed;
    */

    int cycles = 0;
    while (cycles < 70224) {
        cycles += m_cpu->execute_next_instruction();
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(1));

    return 70224;
}

void Syncroniser::attach_components(CPU* cpu, PPU* ppu) {
	m_cpu = cpu;
	m_ppu = ppu;
}

void Syncroniser::real_ticks(int ticks) {
	for (int i = 0; i < ticks; i++) {
		//advance other components
		m_ppu->tick();
	}
}

void Syncroniser::real_cycle() {
	for (int i = 0; i < 4; i++) {
		//advance other components
		m_ppu->tick();
	}
}
