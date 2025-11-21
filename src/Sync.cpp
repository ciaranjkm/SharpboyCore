#include <Sync.h>
#include <ComponentManager.h>

Syncroniser::~Syncroniser() {
	m_cpu = nullptr;
	m_ppu = nullptr;
}

int Syncroniser::advance_cycles() {
	//TODO SYNC TO AUDIO BUFFER, RUN AT 100% CPU SPEED GATHERING AUDIO BUFFER, AND SETTING FLAGS TO LET THE USER KNOW THE BUFFER IS READY
	//FOR DISPLAY VBLANK FOR UPDATE TEXTURE
	//FOR AUDIO USE DOUBLE BUFFER, ONCE GENERATED ONE SECOND, SET FLAG AND RELEASE BUFFER
    int cycles = 0;
    while (cycles < 70224) {
		cycles += m_cpu->step();
    }

    return cycles;
}

void Syncroniser::attach_components(ComponentManager* comp_manager) {
	m_cpu = comp_manager->get_cpu();
	m_timer = comp_manager->get_timer();
	m_ppu = comp_manager->get_ppu();
}

void Syncroniser::real_ticks(int ticks) {
	for (int i = 0; i < ticks; i++) {
		//advance other components
		m_ppu->tick();
		m_timer->tick();
	}
}

void Syncroniser::real_cycle() {
	for (int i = 0; i < 4; i++) {
		//advance other components 1 m cycle
		m_ppu->tick();
		m_timer->tick();
	}
}
