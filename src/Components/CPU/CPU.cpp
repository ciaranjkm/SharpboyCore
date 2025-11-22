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


	//OVERLAP OF FETCH AND INTERRUPT CHECK FROM PREVIOUS INSTRUCTION
	u8 op = read_pc();

	//std::cout << std::format("B:{:#x} C:{:#x}\n", m_registers.b, m_registers.c);
	//std::cout << std::format("A:{:#x}\n", m_registers.a);

	//std::cout << std::format("B:{:#x} C:{:#x} D:{:#x} E:{:#x} H:{:#x} L:{:#x}\n",
		//m_registers.b, m_registers.c, m_registers.d, m_registers.e, m_registers.h, m_registers.l);

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
	//idk why this works but it does it is probably not what actually happens
	
	//T1 ADDRESS ON BUS 
	//T2 READ DATA
	//T3 INTERNAL OPERATION
	//T4 INTERNAL OPERATION

	tick_components(2);			
	u8 value = m_bus->cpu_read(address);	
	tick_components(2);					
	return value;
}

void CPU::write(u16 address, u8 value) {
	//idk why this works but it does it is probably not what actually happens

	//T1 ADDRESS ON BUS
	//T2 WRITE DATA
	//T3 INTERNAL OPERATION
	//T4 INTERNAL OPERATION

	tick_components(2);		
	m_bus->cpu_write(address, value);	
	tick_components(2);					
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
	interrupt_pending = Interrupts::get_pending_interrupt();
	if (!m_cpu.ime || interrupt_pending == interrupt_none) {
		return 0;
	}

	idle_cycle();    
	idle_cycle();

	m_cpu.ime = false;
	m_cpu.halted = false;

	m_registers.sp--;
	write(m_registers.sp--, (m_registers.pc >> 8) & 0xFF);

	//CHECK VECTOR HERE INCASE OF IE/IF PUSH, TOO LATE AFTER LOW BYTE WRITE
	interrupt_pending = Interrupts::get_pending_interrupt();
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