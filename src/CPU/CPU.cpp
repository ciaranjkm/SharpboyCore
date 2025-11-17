#include <CPU/CPU.h>
#include <Sync.h>

CPU::~CPU() {
	m_bus = nullptr;
	m_syncroniser = nullptr;
}

//INITIALISATION
bool CPU::set_bus_ptr(Bus* bus) {
	m_bus = bus;

	if (!m_bus) {
		Logger::log(log_error, "CPU bus given nullptr");
		return false;
	}

	Logger::log(log_debug, "CPU set bus pointer");
	return true;
}

bool CPU::set_timing_ptr(Syncroniser* syncroniser) {
	m_syncroniser = syncroniser;

	if (!m_syncroniser) {
		Logger::log(log_error, "CPU syncroniser given nullptr");
		return false;
	}

	Logger::log(log_debug, "CPU set syncroniser pointer");
	return true;
}

//VIRTUAL FUNCTIONS
u8 CPU::read(u16 address) {
	return m_bus->read(address);
}

void CPU::write(u16 address, u8 value) {
	m_bus->write(address, value);
}

u8 CPU::read_pc(bool read_interrupt) {
	u8 value = read(m_registers.pc);
	m_registers.pc++;
	return value;
}

u16 CPU::read_pc_short() {
	u8 low = read_pc();
	u8 high = read_pc();
	return (high << 8) | low;
}

void CPU::tick_components(int ticks) {
	m_syncroniser->real_ticks(ticks);
}

void CPU::idle_cycle() {
	m_syncroniser->real_cycle();
}
