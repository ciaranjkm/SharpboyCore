#pragma once

#include "../Common.h"

struct s_joypad_state {
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	
	bool start = false;
	bool select = false;

	bool a = false;
	bool b = false;
};

class Joypad {
public:
	void reset();
	
	void set_joypad_state(s_joypad_state state);
	void write_to_joyp(u8 value);
	u8 read_joypad_state();

private:
	u8 joyp_register = 0xcf;
	s_joypad_state current_state;
};
