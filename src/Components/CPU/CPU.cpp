#include <Components/CPU/CPU.h>
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

bool CPU::set_sync_ptr(Syncroniser* syncroniser) {
	m_syncroniser = syncroniser;

	if (!m_syncroniser) {
		Logger::log(log_error, "CPU syncroniser given nullptr");
		return false;
	}

	Logger::log(log_debug, "CPU set syncroniser pointer");
	return true;
}

//VIRTUAL FUNCTIONS
int CPU::step() {
	int cycles = 0;

	u8 op = read_pc();
	cycles += execute_opcode(op);

	cycles += handle_interrupt();

	check_enable_ime();
	return cycles;
}

u8 CPU::read(u16 address) {
	idle_cycle();
	return m_bus->read(address);
}

void CPU::write(u16 address, u8 value) {
	idle_cycle();
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

u8 CPU::nontick_read(u16 address) {
	return m_bus->read(address);
}

void CPU::nontick_write(u16 address, u8 value) {
	m_bus->write(address, value);
}

void CPU::check_for_interrupt() {
	u8 int_flag = Interrupts::read_if();
	u8 int_enable = Interrupts::read_ie();

	m_cpu.interrupt_pending = (int_flag & int_enable) & 0x1f;
}

int CPU::handle_interrupt() {
	if (!m_cpu.ime || !Interrupts::check_for_interrupt()) {
		return 0;
	}

	e_interrupts pending = Interrupts::get_pending_interrupt();
	if (pending == interrupt_none) {
		return 0;
	}

	idle_cycle();    
	idle_cycle();

	m_cpu.ime = false;
	m_cpu.halted = false;

	m_registers.sp--;
	write(m_registers.sp--, (m_registers.pc >> 8) & 0xFF);
	write(m_registers.sp, m_registers.pc & 0xFF);         

	idle_cycle(); 

	Interrupts::clear_interrupt(pending);
	m_registers.pc = Interrupts::get_interrupt_vector(pending);

	return ticks_20; 
}

void CPU::check_enable_ime() {
	if (m_cpu.enable_ime) {
		m_cpu.ime = true;
		m_cpu.enable_ime = false;
	}
}

e_interrupts CPU::get_interrupt_pending() {
	for (int i = 0; i < 5; i++) {
		u8 interrupt_pending = (m_cpu.interrupt_pending & (0x01 << i));
		if (interrupt_pending != 0) {
			return (e_interrupts)i;
		}
	}

	return interrupt_none;
}

void CPU::clear_if_bit(e_interrupts bit_to_clear) {
	u8 mask = ~(0x01 << bit_to_clear);

	u8 interrupt_flag = nontick_read(io_if);
	interrupt_flag &= mask;

	nontick_write(io_if, interrupt_flag);
}