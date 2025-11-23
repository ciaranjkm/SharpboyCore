#include <Sync.h>
#include <ComponentManager.h>

Syncroniser::~Syncroniser() {
	m_cpu = nullptr;
	m_ppu = nullptr;
	m_timer = nullptr;
}

void Syncroniser::start_syncroniser() {
	m_sync.start_clock();
}

int Syncroniser::advance_cycles() {
	//SYNCED WITH CHRONO FOR NOW AT ROUGHLY CLOCK SPEED MAYBE 1% SLOWER, GOOD ENOUGH FOR VIDEO SYNC WILL UPDATE FOR AUDIO ANOTHER TIME
	const auto time_now = std::chrono::high_resolution_clock::now();
	const auto time_delta = time_now - m_sync.previous_time;
	m_sync.previous_time = time_now;

	const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(time_delta).count();
	const uint64_t target_ticks = (nanos * CPU_CLOCK) / 1000000000;

	m_sync.overrun_ticks += target_ticks;

	int ticks_completed = 0;

	while (m_sync.overrun_ticks > 0) {
		int step_ticks = m_cpu->step();
		ticks_completed += step_ticks;
		m_sync.overrun_ticks -= step_ticks;
	}

	if (m_sync.overrun_ticks < -10000) { // If more than 10k cycles ahead
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
	else {
		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}

	return ticks_completed;
}

void Syncroniser::attach_components(ComponentManager* comp_manager) {
	m_cpu = comp_manager->get_cpu();
	m_timer = comp_manager->get_timer();
	m_ppu = comp_manager->get_ppu();
}

void Syncroniser::real_ticks(int ticks) {
	for (int i = 0; i < ticks; i++) {
		//advance other components
		m_ppu->dma_tick();
		m_ppu->tick();
		m_timer->tick();
	}
}

void Syncroniser::real_cycle() {
	for (int i = 0; i < 4; i++) {
		//advance other components 1 m cycle
		m_ppu->dma_tick();
		m_ppu->tick();
		m_timer->tick();
	}
}
