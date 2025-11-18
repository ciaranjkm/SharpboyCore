#pragma once

#include "Utilities/Common.h"

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

static inline u8 convert_to_joypad_value(s_joypad_state& joypad_state) {

	return 0xff;
}