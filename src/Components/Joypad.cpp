#include <Components/Joypad.h>

void Joypad::reset() {
	joyp_register = 0xcf;
	current_state = {};
}

void Joypad::set_joypad_state(s_joypad_state state) {
	current_state = state;
}

void Joypad::write_to_joyp(u8 value) {
	joyp_register = value;
}

u8 Joypad::read_joypad_state() {
	u8 joyp = joyp_register & 0xf0;
	u8 keys = 0x0f;

	if (!(joyp & 0x20)) {
		if (current_state.start)  keys &= ~(1 << 3);
		if (current_state.select) keys &= ~(1 << 2);
		if (current_state.b)      keys &= ~(1 << 1);
		if (current_state.a)      keys &= ~(1 << 0);
	}

	if (!(joyp & 0x10)) {
		if (current_state.down)  keys &= ~(1 << 3);
		if (current_state.up)    keys &= ~(1 << 2);
		if (current_state.left)  keys &= ~(1 << 1);
		if (current_state.right) keys &= ~(1 << 0);
	}

	u8 result = joyp | keys;
	return result;	
}