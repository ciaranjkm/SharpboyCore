#pragma once

enum e_interrupts {
	interrupt_none = -1,
	interrupt_vblank = 0x00,
	interrupt_lcd = 0x01,
	interrupt_timer = 0x02,
	interrupt_serial = 0x03,
	interrupt_joypad = 0x04,
};