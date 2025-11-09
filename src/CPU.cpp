#include "../include/CPU.h"
#include "../include/Timing.h"

CPU::~CPU() {
	m_bus = nullptr;
	m_timing = nullptr;
}

//execution
void CPU::execute_next_instruction() {
	u8 opcode = read(m_registers.pc);
}

//initialisation
bool CPU::set_bus_ptr(Bus* bus) {
	m_bus = bus;

	if (!m_bus) {
		return false;
	}

	return true;
}

bool CPU::set_timing_ptr(Timing* timing) {
	m_timing = timing;

	if (!m_timing) {
		return false;
	}

	return true;
}

void CPU::reset_regs() {
	m_registers.a = 0x00;
	m_registers.f = 0x00;
	m_registers.b = 0x00;
	m_registers.c = 0x00;
	m_registers.d = 0x00;
	m_registers.e = 0x00;
	m_registers.h = 0x00;
	m_registers.l = 0x00;

	m_registers.pc = 0x0000;
	m_registers.sp = 0x0000;
}

void CPU::reset_regs(std::array<u8, 0x08> regs, u16 sp, u16 pc) {
	m_registers.a = regs[0];
	m_registers.f = regs[1];
	m_registers.b = regs[2];
	m_registers.c = regs[3];
	m_registers.d = regs[4];
	m_registers.e = regs[5];
	m_registers.h = regs[6];
	m_registers.l = regs[7];

	m_registers.sp = sp;
	m_registers.pc = pc;
}

//sst
bool CPU::set_sst_ptr(SST_Tester* sst_tester) {
	tester = sst_tester;
	if (tester) {
		sst = true;
	}
	else {
		sst = false;
	}

	return sst;
}

void CPU::reset_sst() {
	tester = nullptr;
	sst = false;
}

//debug
s_registers* CPU::get_registers() {
	return &m_registers;
}

//timing
void CPU::tick_components(int ticks) {
	m_timing->real_ticks(ticks);
}

void CPU::idle_cycle() {
	m_timing->real_ticks(4);
	add_test_cycle(m_registers.pc, 0x00, "---");
}

//memory access
u8 CPU::read(u16 address) {
	add_test_cycle(address, 0x00, "r-m");
	return m_bus->read(address);
}

void CPU::write(u16 address, u8 value) {
	m_bus->write(address, value);
	add_test_cycle(address, value, "w-m");
}

//flag helpers
bool CPU::get_flag(e_flags flag) {
	return (m_registers.f >> flag) & 0x1;
}

void CPU::set_flag(e_flags flag, bool state) {
	if (state) {
		m_registers.f |= ((7 - flag) << state);
	}
	else {
		m_registers.f &= ~(1 << (7 - flag));
	}
}

//register helpers

u16 CPU::get_joined_reg(e_joined_regs reg) {
	switch (reg) {
	case rAF:
		return (u16)(m_registers.a << 8 | m_registers.f);

	case rBC:
		return (u16)(m_registers.b << 8 | m_registers.c);

	case rDE:
		return (u16)(m_registers.d << 8 | m_registers.e);

	case rHL:
		return (u16)(m_registers.h << 8 | m_registers.l);

	default:
		return 0xffff;
	}
}

void CPU::set_joined_reg(e_joined_regs reg, u16 value) {
	u8 low = value & 0xff;
	u8 high = value >> 8;

	switch (reg) {
	case rAF:
		m_registers.a = high;
		m_registers.f = low & 0xf0;
		return;

	case rBC:
		m_registers.b = high;
		m_registers.c = low;
		return;

	case rDE:
		m_registers.d = high;
		m_registers.e = low;
		return;

	case rHL:
		m_registers.h = high;
		m_registers.l = low;

	default:
		return;
	}
}

//sst
void CPU::add_test_cycle(u16 address, u8 value, std::string operation) {
	if (sst) {
		if (tester) {
			tester->add_cycle(address, value, operation);
		}
	}
}
