#include <Sync.h>
#include <ComponentManager.h>

Syncroniser::~Syncroniser() {
	m_cpu = nullptr;
	m_ppu = nullptr;
	m_timer = nullptr;
}

int Syncroniser::advance_cycles() {
    const auto time_now = std::chrono::high_resolution_clock::now();

    if (m_sync.frame_count == 0) {
        m_sync.start_time = time_now;
        m_sync.previous_time = time_now;
        m_sync.overrun_ticks = 0;
        m_sync.total_target_ticks = 0;
        m_sync.total_actual_ticks = 0;
        m_sync.avg_drift_cycles = 0.0;
    }

    const auto time_delta = time_now - m_sync.previous_time;
    m_sync.previous_time = time_now;

    const int64_t nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(time_delta).count();

    //check for 100ms or more nanoseconds elapsed and clamp if more
    constexpr int64_t MAX_FRAME_NANOS = 100000000;
    const int64_t clamped_nanos = std::min(nanos, MAX_FRAME_NANOS);

    constexpr int64_t NANOS_PER_SEC = 1000000000LL;
    const int64_t target_ticks = (clamped_nanos * static_cast<int64_t>(CPU_CLOCK)) / NANOS_PER_SEC;

    m_sync.overrun_ticks += target_ticks;
    m_sync.total_target_ticks += target_ticks;

    int ticks_completed = 0;

    // Execute CPU steps until we've caught up
    while (m_sync.overrun_ticks > 0) {
        int step_ticks = m_cpu->step();
        ticks_completed += step_ticks;
        m_sync.overrun_ticks -= step_ticks;
    }

    m_sync.total_actual_ticks += ticks_completed;
    m_sync.frame_count++;

    //if we are running fast sleep to throttle the speed
    if (m_sync.overrun_ticks < 0) {
        const int64_t cycles_ahead = -m_sync.overrun_ticks;
        const int64_t nanos_ahead = (cycles_ahead * NANOS_PER_SEC) / static_cast<int64_t>(CPU_CLOCK);

        constexpr int64_t MIN_SLEEP_NANOS = 500000; 

        if (nanos_ahead > MIN_SLEEP_NANOS) {
            const auto sleep_duration = std::chrono::nanoseconds(nanos_ahead * 7 / 10); //leave headroom for os sleep overhead
            std::this_thread::sleep_for(sleep_duration);
        }
        else if (nanos_ahead > 100000) { 
            std::this_thread::yield();
        }

        //check if we are more then 100ms ahead and clamp if we are to 100ms of ticks
        constexpr int64_t MAX_AHEAD_CYCLES = static_cast<int64_t>(CPU_CLOCK) / 10; 
        if (cycles_ahead > MAX_AHEAD_CYCLES) {
            m_sync.overrun_ticks = -MAX_AHEAD_CYCLES;
        }
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
        m_timer->tick();
		m_ppu->dma_tick();
		m_ppu->tick();
	}
}

void Syncroniser::real_cycle() {
	for (int i = 0; i < 4; i++) {
		//advance other components 1 m cycle
        m_timer->tick();
		m_ppu->dma_tick();
		m_ppu->tick();
	}
}
