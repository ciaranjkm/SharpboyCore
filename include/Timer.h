#pragma once

#include "Utilities/Common.h"
#include "Utilities/IO.h"

struct s_timer_io {
	u16 div = 0x00;
	u8 tima = 0x00;
	u8 tma = 0x00;
	u8 tac = 0x00;
};

class Timer {
public:
	//TICK
	void tick();

	//IO ACCESS
	u8 read_io(u16 address);
	void write_io(u16 address, u8 value);

private:
	//MEMBER VARIABLES
	s_timer_io m_timer_io = {};
};