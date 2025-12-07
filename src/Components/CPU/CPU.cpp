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
		return false;
	}

	return true;
}

bool CPU::set_sync_ptr(Syncroniser* syncroniser) {
	m_syncroniser = syncroniser;

	if (!m_syncroniser) {
		return false;
	}

	return true;
}

//VIRTUAL FUNCTIONS
int CPU::step() {
	int cycles = 0;

	if (m_cpu.halted) {
		Interrupts::get_new_pending();
		int intr_handled = handle_interrupt();
		if (intr_handled > 0) {
			return intr_handled;
		}
		else {
			tick_components(1);
			return 1;
		}
	}

	u8 op = read_pc();
	check_halt_bug();

	cycles += execute_opcode(op);
	cycles += handle_interrupt();

	check_enable_ime();
	return cycles;
}

u8 CPU::read(u16 address) { 
	tick_components(2);			
	u8 value = m_bus->cpu_read(address);	
	tick_components(2);					
	return value;
}

void CPU::write(u16 address, u8 value) {
	tick_components(2);		
	m_bus->cpu_write(address, value);	
	tick_components(2);					
}

u8 CPU::read_pc(bool read_interrupt) {
	tick_components(2);
	u8 value = m_bus->cpu_read(m_registers.pc++);
	tick_components(2);
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

int CPU::handle_interrupt() {
	if (Interrupts::get_new_pending() == interrupt_none) {
		return 0;
	}

	m_cpu.halted = false;

	if (!m_cpu.ime) {
		return 0;
	}

	idle_cycle();
	idle_cycle();

	m_cpu.halted = false;
	m_cpu.ime = false;

	m_registers.sp--;
	write(m_registers.sp--, (m_registers.pc >> 8) & 0xFF);

	//CHECK VECTOR HERE INCASE OF IE/IF PUSH, TOO LATE AFTER LOW BYTE WRITE
	interrupt_pending = Interrupts::get_new_pending();
	u16 vector = Interrupts::get_interrupt_vector(interrupt_pending);

	write(m_registers.sp, m_registers.pc & 0xFF);         
	idle_cycle(); 

	if (vector != 0x00) {
		m_registers.pc = vector;
		Interrupts::clear_interrupt(interrupt_pending);

		return ticks_20;
	}

	m_registers.pc = 0x0000;
	return ticks_20; 
}

void CPU::check_enable_ime() {
	if (m_cpu.enable_ime) {
		m_cpu.ime = true;
		m_cpu.enable_ime = false;
	}
}

void CPU::check_halt_bug() {
	if (m_cpu.halt_bug) {
		m_registers.pc--;
		m_cpu.halt_bug = false;
	}
}