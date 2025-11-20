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

	if (m_cpu.halted) {
		if (Interrupts::check_for_interrupt()) {
			m_cpu.halted = false;
			return 0;
		}
		else {
			tick_components(1);
			return cycles += 1;
		}
	}

	u8 op = read_pc();

	//DEBUG OUT FOR MOONEYE TEST ROMS PASS/ FAIL 
	//3 5 8 pass
	//42 42 42 fail
	if (op == inst_LD_B_B) {
		std::cout << std::format("B:{:#x} C:{:#x} D:{:#x} E:{:#x} H:{:#x} L:{:#x}\n",
			m_registers.b, m_registers.c, m_registers.d, m_registers.e, m_registers.h, m_registers.l);
		return 0;
	}
	check_halt_bug();

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

int CPU::handle_interrupt() {
	if (!m_cpu.ime) {
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

	u16 vector = Interrupts::get_interrupt_vector(pending);
	if (vector != 0x00) {
		m_registers.pc = vector;
		Interrupts::clear_interrupt(pending);

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