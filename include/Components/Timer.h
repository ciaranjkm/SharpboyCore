#pragma once

#include "../Common.h"
#include "IO.h"

class CPU;

struct s_timer_io {
	u16 div = 0x18;
	u8 tima = 0x00;
	u8 tma = 0x00;
	u8 tac = 0x00;
};

class Timer {
public:
	//TICK
	void tick(CPU* m_cpu);

	//RESET
	void reset(bool using_boot_rom = false);

	//IO ACCESS
	u8 read_io(u16 address);
	void write_io(u16 address, u8 value);

private:
	//MEMBER VARIABLES
	s_timer_io m_timer_io = {};

	bool reload_tima = false;
	int tima_delay = 0;
	bool previous_and_result = false;

	const int DEFAULT_TIMA_DELAY = 8;
};