#pragma once
#include "Utilities/Common.h"
#include "Utilities/Testing.h"
#include <array>

#include "Bus.h"

struct s_registers {
	u8 a, f, b, c, d, e, h, l;
	u16 pc, sp;
};

enum e_flags {
	NO_FLAG = -1,
	fZERO = 0,
	fSUB = 1,
	fHCARRY = 2,
	fCARRY = 3
};

enum e_joined_regs {
	rAF,
	rBC,
	rDE,
	rHL
};

class Timing;

class CPU {
public:
	~CPU();

	//execution
	void execute_next_instruction();

	//initialisation
	bool set_bus_ptr(Bus* bus);
	bool set_timing_ptr(Timing* timing);
	void reset_regs();
	void reset_regs(std::array<u8, 0x08> regs, u16 sp, u16 pc);

	//sst
	bool set_sst_ptr(SST_Tester* sst_tester);
	void reset_sst();

	//debug
	s_registers* get_registers();

private:
	s_registers m_registers;
	Bus* m_bus = nullptr;
	Timing* m_timing = nullptr;

	//TESTING PTR
	bool sst = false;
	SST_Tester* tester = nullptr;

private:
	//timing
	void tick_components(int ticks);
	void idle_cycle();

	//memory access
	u8 read(u16 address);
	void write(u16 address, u8 value);

	//flag helpers 
	bool get_flag(e_flags flag);
	void set_flag(e_flags flag, bool state);

	//register helpers
	u16 get_joined_reg(e_joined_regs reg);
	void set_joined_reg(e_joined_regs reg, u16 value);

	//sst
	void add_test_cycle(u16 address, u8 value, std::string operation);
};