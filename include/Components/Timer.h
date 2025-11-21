#pragma once

#include "../Common.h"
#include "Interrupts.h"
#include "IO.h"

struct s_timer_io {
	u16 sys_clock = 0x00;
	u8 tima = 0x00;
	u8 tma = 0x00;
	u8 tac = 0x00;
};

const int DEFAULT_TIMA_DELAY = 8;

/*
	Class for the timers in the Game Boy, originally started with my original timer but used
	https://github.com/raddad772/jsmoo/blob/main/system/gb/gb_cpu.js for inspo with this design
	
	PASSES ALL MOONEYE TEST ROMS FOR TIMERS + BLARGGS MEM/INSTR TIMING

*/

class Timer {
public:
	//TICK
	void tick();

	//RESET
	void reset(bool using_boot_rom = false);

	//IO ACCESS
	u8 read_io(u16 address);
	void write_io(u16 address, u8 value);

private:
	s_timer_io m_timer_io = {};

	u8 previous_timer_bit = 0x00;

	bool m_tima_reload_this_cycle = false;
	int m_tima_ticks_till_irq = 0;
	int m_tima_ticks_till_finish = 0;

private:
	//MEMBER FUNCTIONS
	void update_sys_clock(u16 new_clock);	
	void handle_tac_write(u8 value);

	u8 get_sys_clock_bit();

	u8 is_tac_enabled();
	void detect_edge_case(u8 previous, u8 current);

	void start_tima_interrupt();
};